## Example CAN logger systemd service
```ini
[Service]
Type=simple
[Unit]
Description=FRUCD CAN Logger
After=network.target

[Service]
Type=simple
ExecStart=/home/frucd/Projects/Raspi-TelemHost-Firmware/services/daq/build/Logger
User=frucd
WorkingDirectory=/home/frucd/Projects/Raspi-TelemHost-Firmware

[Install]
WantedBy=multi-user.target
```