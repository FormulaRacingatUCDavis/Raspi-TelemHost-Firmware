#!/bin/bash

# Set up virtual CAN for testing
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
sudo ip link add dev vcan1 type vcan
sudo ip link set up vcan1

# Set up CAN channels
sudo ip link set can0 up type can bitrate 500000
sudo ip link set can1 up type can bitrate 500000