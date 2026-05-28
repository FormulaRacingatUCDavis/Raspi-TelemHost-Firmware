#!/bin/bash

cd /home/frucd/Raspi-TelemHost-Firmware/

echo "generating new dbc source files an doverwritting old ones"
python3 -m cantools generate_c_source --database-name cm200_db --output-directory dashboard/src "resources/20240129 Gen5 CAN DB.dbc"
python3 -m cantools generate_c_source --database-name cm200_db --output-directory services/daq/src "resources/20240129 Gen5 CAN DB.dbc"
python3 -m cantools generate_c_source --database-name fe13_db --output-directory dashboard/src resources/FE12.dbc && mv dashboard/src/*.h dashboard/inc
python3 -m cantools generate_c_source --database-name fe13_db --output-directory services/daq/src resources/FE12.dbc && mv services/daq/src/*.h services/daq/include
echo "finished updating dbc source files"

echo "rebuilding driver dashboard files"
cd /home/frucd/Raspi-TelemHost-Firmware/dashboard/build
cmake ..
cmake --build .
echo "finished building driver dashboard files"

echo "rebuilding web app files"
cd /home/frucd/Raspi-TelemHost-Firmware/services/daq
cmake -S . -B build \ 
    -DCMAKE_C_COMPILER=gcc \ 
    -DCMAKE_CXX_COMPILER=g++ \ 
    -DCMAKE_TOOLCHAIN_FILE="/home/frucd/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build
echo "finished rebuilding web app files"

cd /home/frucd/Raspi-TelemHost-Firmware/
