import os
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
import json
import subprocess

app = FastAPI()

with open('config/config.json', "r") as f:
    config = json.load(f)

app.mount("/static", StaticFiles(directory='services/web/public'), name="static")

@app.post("/toggle_log")
async def toggle_log():
    result = subprocess.run(
        ["sudo", "systemctl", "is-active", "can-logger.service"],
        capture_output=True,
        text=True
    )
    active = result.stdout.strip() == "active"

    if active:
        subprocess.run(["sudo", "systemctl", "stop", "can-logger.service"])
    else:
        subprocess.run(["sudo", "systemctl", "start", "can-logger.service"])

    return {"logging": not active}
    
@app.get("/log_status")
async def log_status():
    result = subprocess.run(
        ["sudo", "systemctl", "is-active", "can-logger.service"],
        capture_output=True,
        text=True
    )
    active = result.stdout.strip() == "active"
    return {"logging": active}

@app.get("/list_logs")
async def list_logs():
    try:
        files = os.listdir('logs')
        csv_files = [f for f in files if f.endswith(".csv")]
        return {"files": csv_files}
    except FileNotFoundError:
        return {"files": [], "error": "Folder not found"}

@app.get("/{full_path:path}")
async def catch_all(full_path: str):
    return FileResponse('services/web/public/index.html')