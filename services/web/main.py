from fastapi import FastAPI, HTTPException
from fastapi.responses import StreamingResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel
from pathlib import Path
from io import BytesIO
from zipfile import ZipFile, ZIP_DEFLATED
from typing import List
import os
import json

app = FastAPI()

PROJ_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

with open(os.path.join(PROJ_DIR, "resources", "config.json"), "r") as f:
    config = json.load(f)

class FileRequest(BaseModel):
    filenames: List[str]

@app.post("/api/canlogs")
async def download_files(payload: FileRequest):
    raw_dir = Path(config["can"]["rawPath"])
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

@app.get("/api/canlog-list")
async def get_log_list():
    return [{"file_name": file} for file in os.listdir(config["can"]["rawPath"])]

app.mount("/resources", StaticFiles(directory=os.path.join(PROJ_DIR, "resources")), name="resources")
app.mount("/", StaticFiles(directory="dist", html=True), name="static")