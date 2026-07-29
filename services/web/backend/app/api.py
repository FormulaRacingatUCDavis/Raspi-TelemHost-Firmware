from fastapi import FastAPI, HTTPException
from fastapi.responses import StreamingResponse
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from pathlib import Path
from io import BytesIO
from zipfile import ZipFile, ZIP_DEFLATED
from typing import List
import contextlib
from contextlib import asynccontextmanager
import os
import json
import asyncio
from watchfiles import awatch, Change
from .canhelper import CANHelper

class FileRequest(BaseModel):
    filenames: List[str]

RESOURCES_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "resources"))
with open(os.path.join(RESOURCES_DIR, "config.json"), "r") as f:
    config = json.load(f)
can_helper = CANHelper(config)
database_pool = None
processing_files = set()

os.makedirs(config["paths"]["data"]["can"]["intake"], exist_ok=True)
os.makedirs(config["paths"]["data"]["can"]["process"], exist_ok=True)
os.makedirs(config["paths"]["data"]["can"]["raw"], exist_ok=True)
os.makedirs(config["paths"]["data"]["can"]["parsed"], exist_ok=True)

semaphore = asyncio.Semaphore(1)
async def watcher(app: FastAPI):
    global processing_files
    async for changes in awatch(config["paths"]["data"]["can"]["process"]):
        for change, path in changes:
            if change == Change.added and path.endswith(".csv"):
                if path not in processing_files:
                    processing_files.add(path)
                    asyncio.create_task(handle_file(path, app))

async def handle_file(path: str, app: FastAPI):
    try:
        async with semaphore:
            loop = asyncio.get_running_loop()
            await loop.run_in_executor(None, can_helper.generate_parsed, path)
    except Exception as e:
        print(f"Failed to process {path}: {e}")  

@asynccontextmanager
async def lifespan(app: FastAPI):
    watcher_task = asyncio.create_task(watcher(app))
    try:
        yield
    finally:
        watcher_task.cancel()
        with contextlib.suppress(asyncio.CancelledError):
            await watcher_task

app = FastAPI(lifespan=lifespan)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:5173"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/")
async def root():
    return {"status": "ok"}

@app.post("/api/can/logs/zip/{type}")
def zip_logs(type: str, payload: FileRequest):
    log_dir = Path(config["paths"]["data"]["can"][type])
    memory_file = BytesIO()
    with ZipFile(memory_file, mode="w", compression=ZIP_DEFLATED) as zf:
        for filename in payload.filenames:
            file_path = (log_dir / filename).resolve(strict=True)
            if log_dir not in file_path.parents and file_path != log_dir:
                raise HTTPException(status_code=400, detail=f"Invalid filename: {filename}")
            zf.write(file_path, arcname=file_path.name)
    memory_file.seek(0)

    return StreamingResponse(
        memory_file,
        media_type="application/zip",
        headers={"Content-Disposition": 'attachment; filename="canlogs.zip"'}
    )

@app.get("/api/{source}/logs/list/{type}")
async def get_log_list(source:str, type: str):
    path = Path(config["paths"]["data"][source][type])
    logs = []
    for file in path.iterdir():
        if file.is_file():
            stats = file.stat()
            logs.append({"file_name": file.name, "creation_date": stats.st_mtime, "file_size": stats.st_size})
    logs.sort(key=lambda x: x["creation_date"], reverse=True)

    return logs