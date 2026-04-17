#!/bin/bash

cd /home/frucd/Raspi-TelemHost-Firmware/
./services/daq/build/DAQ

cd web
source venv_webapp/bin/activate
python -m uvicorn main:app --host 192.168.105.2