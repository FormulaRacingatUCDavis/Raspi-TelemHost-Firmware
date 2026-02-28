#!/bin/bash

cd /home/frucd/projects/Raspi-TelemHost-Firmware/services/
./daq/build/DAQ

cd web
source ../.venv/bin/activate
python -m uvicorn main:app --host 0.0.0.0