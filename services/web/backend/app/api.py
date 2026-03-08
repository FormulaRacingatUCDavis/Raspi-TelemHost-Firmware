from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse, StreamingResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel
from pathlib import Path
from io import BytesIO
from zipfile import ZipFile, ZIP_DEFLATED
from typing import List

import csv
import cantools
import os
import json

class FileRequest(BaseModel):
    filenames: List[str]

app = FastAPI()

RESOURCES_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "resources"))
with open(os.path.join(RESOURCES_DIR, "config.json"), "r") as f:
    config = json.load(f)

frucd_db = cantools.database.load_file(os.path.join(RESOURCES_DIR, "20240129 Gen5 CAN DB.dbc"))
mc_db = cantools.database.load_file(os.path.join(RESOURCES_DIR, "FE12.dbc"))
os.makedirs(config["paths"]["data"]["intake"], exist_ok=True)
os.makedirs(config["paths"]["data"]["raw"], exist_ok=True)
os.makedirs(config["paths"]["data"]["processed"], exist_ok=True)

@app.post("/api/can/raw-logs/zip")
async def zip_logs(payload: FileRequest):
    raw_dir = Path(config["paths"]["data"]["raw"])
    memory_file = BytesIO()
    with ZipFile(memory_file, mode="w", compression=ZIP_DEFLATED) as zf:
        for filename in payload.filenames:
            file_path = (raw_dir / filename).resolve(strict=True)
            if raw_dir not in file_path.parents and file_path != raw_dir:
                raise HTTPException(status_code=400, detail=f"Invalid filename: {filename}")
            zf.write(file_path, arcname=file_path.name)
    memory_file.seek(0)

    return StreamingResponse(
        memory_file,
        media_type="application/zip",
        headers={"Content-Disposition": 'attachment; filename="canlogs.zip"'}
    )

@app.post("/api/can/raw-logs/load")
async def load_logs():
    for log in os.listdir(config["paths"]["data"]["raw"]):
        if os.path.exists(os.path.join(config["paths"]["data"]["processed"], log)):
            return
        
        parsed_msgs = []
        ids = set()
        signals = set()
        f_ids = set()
        f_count = 0

        print(f'[LOG PARSER] Parsing {log}...')
        with open(os.path.join(config["paths"]["data"]["raw"], log), 'r', newline='') as log_raw:
            reader = csv.reader(log_raw)
            for row in reader:
                id_str = row[0]
                id = int(id_str, 16)
                message = None
                for dbc in [frucd_db, mc_db]:
                    try:
                        message = dbc.get_message_by_frame_id(id)
                        break
                    except KeyError:
                        continue
                if message is None:
                    print(f'[LOG PARSER] >> Unrecognized ID {id} at timestamp: {row[-1]}')
                    f_ids.add(id)
                    f_count += 1
                    continue

                data_bytes = []
                for b in row[1:9]:
                    if b.strip() == "":
                        data_bytes.append(0)
                    else:
                        try:
                            data_bytes.append(int(b))
                        except ValueError:
                            data_bytes.append(0)

                raw_data = bytes(data_bytes)
                timestamp = float(row[-1]) / 1000.0
                parsed_data = message.decode(raw_data)
                
                parsed_msgs.append((
                    timestamp,
                    message.senders[0],
                    id_str,
                    message.name,
                    parsed_data
                ))

                if id_str not in ids:
                    ids.add(id_str)
                    for signal in message.signals:
                        signals.add(signal.name)

        with open(os.path.join(config["paths"]["data"]["processed"], log), 'w', newline='') as log_parsed:
            signals_list = sorted(signals)
            header = ['Timestamp [s]', 'Source', 'ID', 'Message'] + signals_list
            writer = csv.writer(log_parsed)
            writer.writerow(header)

            for timestamp, source, id_str, msg_name, decoded in parsed_msgs:
                row_data = [timestamp, source, id_str, msg_name]
                for sig in signals_list:
                    value = decoded.get(sig, "")
                    row_data.append(value)
                writer.writerow(row_data)

            print(f'[LOG PARSER] >> Total failed rows: {f_count}')
        
    return {"status": "complete"}

@app.get("/api/can/raw-logs/list")
async def get_log_list():
    return [{"file_name": file} for file in os.listdir(config["paths"]["data"]["raw"])]

app.mount("/_app", StaticFiles(directory=config["paths"]["web"]["_app"]), name="app")

@app.get("/{full_path:path}")
async def spa_fallback(full_path: str):
    file_path = os.path.join(config["paths"]["web"]["build"], full_path)
    if os.path.isfile(file_path):
        return FileResponse(file_path)

    return FileResponse(os.path.join(config["paths"]["web"]["build"], "index.html"))