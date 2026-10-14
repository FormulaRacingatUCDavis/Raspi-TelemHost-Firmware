#!/bin/bash

# Display GUI to LCD screen
export DISPLAY=:0
for i in {1..20}; do
    if [ -S /tmp/.X11-unix/X0 ]; then
        echo "Display ready"
        break
    fi
    echo "Waiting for display"
    sleep 2
done

# Run dashboard if display available, else fallback
cd /home/frucd/Raspi-TelemHost-Firmware
if [ -S /tmp/.X11-unix/X0 ]; then
    echo "Starting dashboard"
    ./dashboard/build/driver_dashboard
else
    echo "No display found"
fi