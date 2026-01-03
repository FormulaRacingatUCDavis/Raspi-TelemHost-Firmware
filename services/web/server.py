import os
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
import json

app = FastAPI()

with open('config/config.json', "r") as f:
    config = json.load(f)

app.mount("/static", StaticFiles(directory='services/web/public'), name="static")

@app.get("/list_logs")
async def list_logs():
    try:
        files = os.listdir('logs')
        return {"files": files}
    except FileNotFoundError:
        return {"files": [], "error": "Folder not found"}

@app.get("/{full_path:path}")
async def catch_all(full_path: str):
    return FileResponse('services/web/public/index.html')