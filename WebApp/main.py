from fastapi import FastAPI, WebSocket
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from services import CANEngine
import asyncio
import cantools.database
from datetime import datetime
import subprocess
import os

app = FastAPI()
engine = CANEngine()

frucd_db = cantools.database.load_file('FE12.dbc')
mc_db = cantools.database.load_file('20240129 Gen5 CAN DB.dbc')

@app.post("/sync-time")
async def sync_time(now: str, tz: str):
    dt = datetime.fromisoformat(now.replace("Z", "+00:00"))
    formatted = dt.strftime("%Y-%m-%d %H:%M:%S")
    subprocess.run(["sudo", "timedatectl", "set-timezone", tz])
    subprocess.run(["sudo", "date", "-s", formatted])
    return {"status": "ok", "time_set": formatted, "timezone": tz}

@app.post('/start')
async def start_logging():
    if engine.start_log():
        return {'status': 'started'}
    return {'status': 'already running'}

@app.post('/stop')
async def stop_logging():
    engine.stop_log()
    return {'status': 'stopped'}

@app.get('/status')
async def status():
    return {
        'running': not engine.stop.is_set(),
        'pcan_log': engine.pcan_reader.log_path if engine.pcan_reader else None,
        'tcan_log': engine.tcan_reader.log_path if engine.tcan_reader else None
    }

@app.get("/download/{bus}")
async def download_csv(bus: str):
    if not engine.pcan_reader or not engine.tcan_reader:
        return {"error": "no logs"}
    if bus == "pcan":
        path = engine.pcan_reader.log_path
    elif bus == "tcan":
        path = engine.tcan_reader.log_path
    else:
        return {"error": "invalid bus"}
    if not path or not os.path.exists(path):
        return {"error": "missing"}
    return FileResponse(path, media_type="text/csv", filename=os.path.basename(path))

@app.websocket('/ws/{bus}/{frame_id}/{signal}')
async def ws_can_stream(websocket: WebSocket, bus: str, frame_id: str, signal: str):
    await websocket.accept()
    prev_ts = None
    requested_id = int(frame_id)
    while True:
        latest = engine.latest_pcan if bus == 'pcan' else engine.latest_tcan
        if latest['id'] != requested_id:
            await asyncio.sleep(0.01)
            continue
        msg_def = None
        for db in (frucd_db, mc_db):
            try:
                msg_def = db.get_message_by_frame_id(latest['id'])
                break
            except KeyError:
                pass
        if not msg_def:
            await asyncio.sleep(0.01)
            continue
        decoded = msg_def.decode(bytes(latest['data']))
        if signal not in decoded:
            await asyncio.sleep(0.01)
            continue
        if latest['timestamp'] != prev_ts:
            await websocket.send_json({
                'timestamp': latest['timestamp'],
                'value': decoded[signal]
            })
            prev_ts = latest['timestamp']
        await asyncio.sleep(0.01)

app.mount('/', StaticFiles(directory='Frontend', html=True), name='static')