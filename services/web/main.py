import cv2
import asyncio
import os

from fastapi import FastAPI, Request, status, HTTPException, WebSocket, WebSocketDisconnect
from fastapi.templating import Jinja2Templates
from fastapi.staticfiles import StaticFiles
from websockets.exceptions import ConnectionClosed
from starlette.responses import FileResponse

from schemas import FilePostCreate, FilePostResponse

app = FastAPI()
app.mount("/static", StaticFiles(directory="static", html=True), name="static")
app.mount("/images", StaticFiles(directory="images"), name="images")
app.mount("/config", StaticFiles(directory=os.path.join(os.path.dirname(__file__), "..", "..", "resources")), name="config")

# RaspPI Camera
# TODO LINK WITH RASPI, using picamera
# https://pip-assets.raspberrypi.com/categories/652-raspberry-pi-camera-module-2/documents/RP-008156-DS-2-picamera2-manual.pdf?disposition=inline
# high level api page
# camera = cv2.VideoCapture(0) 

templates = Jinja2Templates(directory="public/templates")

file_posts: list[dict] = [{}]

BASE_DIR = os.path.dirname(__file__)
FILE_DIR = os.path.abspath(os.path.join(BASE_DIR, "..", "..", "testlogs"))

def create_file(file_name):
    new_id = max(p["id"] for p in file_posts) + 1 if file_posts else 1
    new_post = {
        "id": new_id,
        "file_name": f"{file_name}",
    }
    file_posts.append(new_post)
    return new_post

def update_file_post():
    file_list = os.listdir(f"{FILE_DIR}")
    file_posts.clear()
    for i in range(len(file_list)):
        create_file(file_list[i])

@app.get("/", include_in_schema=False)
async def home(request: Request):
    return templates.TemplateResponse(request, "home.html", {"file_posts": file_posts})

@app.get("/camera", include_in_schema=False)
async def cameraPage(request: Request):
    return templates.TemplateResponse(request, "camera.html", {})

@app.websocket("/ws")
async def get_stream(websocket: WebSocket):
    await websocket.accept()
    print("Client connected")
    try:
        while True:
            success, frame = camera.read()
            if not success:
                print("Failed to grab frame")
                break
            else:
                ret, buffer = cv2.imencode('.jpg', frame)
                await websocket.send_bytes(buffer.tobytes()) 
            await asyncio.sleep(0.03)
    except (WebSocketDisconnect, ConnectionClosed):
        print("Client disconnected")    

@app.get("/api/update/dropdown-list", include_in_schema=False)
async def update_dropdown_list(request: Request):
    update_file_post()
    return templates.TemplateResponse(request, "dropdown-list.html", {"file_posts": file_posts})

@app.get("/api/file_posts", include_in_schema=False)
def get_file_post():
    return file_posts

@app.get("/api/file_post/{file_post_id}", response_model=FilePostResponse)
def get_file_post(file_post_id: int):
    for post in file_posts:
        if post.get("id") == file_post_id:
            return post
    raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="File could not be found.")

# TODO: Currently gets logs from relative logs directory in /services/web directory (make it path to daq logs)
@app.get("/api/download/{file_post_id}")
def downloadFile(file_post_id: int):
    for post in file_posts:
        if post.get("id") == file_post_id:
            file_name = post["file_name"]
            file_location = f"{FILE_DIR}{file_name}"
            return FileResponse(file_location, media_type='application/octet-stream',filename=file_name)
    raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="File could not be found.")

