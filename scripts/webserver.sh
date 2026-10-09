#!/bin/bash

cd /home/frucd/Raspi-TelemHost-Firmware/
./services/daq/build/DAQ &

source venv_webapp/bin/activate
python3 services/web/backend/main.py