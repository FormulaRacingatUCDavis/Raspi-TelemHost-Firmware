from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse, StreamingResponse
from fastapi.staticfiles import StaticFiles
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

semaphore = asyncio.Semaphore(1)
async def watcher():
    async for changes in awatch(config["paths"]["data"]["process"]):
        for change, path in changes:
            if change == Change.added and path.endswith(".csv"):
                asyncio.create_task(handle_file(path))

async def handle_file(path: str):
    async with semaphore:
        loop = asyncio.get_running_loop()
        await loop.run_in_executor(None, can_helper.generate_parsed, path)

@asynccontextmanager
async def lifespan(app: FastAPI):
    watcher_task = asyncio.create_task(watcher())
    try:
        yield
    finally:
        watcher_task.cancel()
        with contextlib.suppress(asyncio.CancelledError):
            await watcher_task

app = FastAPI(lifespan=lifespan)

@app.post("/api/can/logs/zip/{type}")
def zip_logs(type: str, payload: FileRequest):
    log_dir = Path(config["paths"]["data"][type])
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

@app.get("/api/can/logs/list")
async def get_log_list():
    path = Path(config["paths"]["data"]["raw"])
    logs = []
    for file in path.iterdir():
        if file.is_file():
            stats = file.stat()
            logs.append({"file_name": file.name, "creation_date": stats.st_mtime, "file_size": stats.st_size})
    logs.sort(key=lambda x: x["creation_date"], reverse=True)

    return logs

app.mount("/_app", StaticFiles(directory=config["paths"]["web"]["_app"]), name="app")

@app.get("/{full_path:path}")
async def spa_fallback(full_path: str):
    file_path = os.path.join(config["paths"]["web"]["build"], full_path)
    if os.path.isfile(file_path):
        return FileResponse(file_path)

    return FileResponse(os.path.join(config["paths"]["web"]["build"], "index.html"))