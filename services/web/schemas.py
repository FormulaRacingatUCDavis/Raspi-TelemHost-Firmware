from pydantic import BaseModel, ConfigDict, Field

class FilePostBase(BaseModel):
    file_name: str

class FilePostCreate(FilePostBase):
    pass

class FilePostResponse(FilePostBase):
    model_config = ConfigDict(from_attributes=True)

    id: int

