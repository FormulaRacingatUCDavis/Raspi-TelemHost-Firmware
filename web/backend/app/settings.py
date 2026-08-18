from pydantic_settings import BaseSettings, SettingsConfigDict

class Settings(BaseSettings):
    pcan_interface: str
    tcan_interface: str

    can_intake_path: str
    can_process_path: str
    can_raw_path: str
    can_parsed_path: str

    config_path: str
    dbc_dir: str

    model_config = SettingsConfigDict(
        env_file=".env",
        env_file_encoding="utf-8"
    )

settings = Settings()