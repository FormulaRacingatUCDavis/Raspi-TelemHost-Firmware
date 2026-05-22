#!/bin/bash

cd /home/frucd/Raspi-TelemHost-Firmware/

echo "removing old dbc source files"
rm dashboard/src/cm200_db.c dashboard/inc/cm200_db.h dashboard/src/fe13_db.c dashboard/inc/fe13_db.h services/daq/src/cm200_db.c services/daq/include/cm200_db.h services/daq/src/fe13_db.c services/daq/inlude/fe13_db.h

echo "generating new dbc source files"
python3 -m cantools generate_c_source --database-name cm200_db --output-directory dashboard/src resources/”20240129 Gen5 CAN DB.dbc”
python3 -m cantools generate_c_source --database-name cm200_db --output-directory services/daq/src resources/”20240129 Gen5 CAN DB.dbc”
python3 -m cantools generate_c_source --database-name fe13_db --output-directory dashboard/src resources/FE12.dbc && mv dashboard/src/*.h dashboard/inc
python3 -m cantools generate_c_source --database-name fe13_db --output-directory services/daq/src resources/FE12.dbc && mv services/daq/src/*.h services/daq/inc

echo "finished updating dbc source files"

echo "rebuilding driver dashboard files"
cd dashboard/build
cmake ..
cmake --build .
cd ../..
echo "finished building driver dashboard files"

echo "rebuilding web app files"
cd services/daq
cmake -S . -B build \ 
    -DCMAKE_C_COMPILER=gcc \ 
    -DCMAKE_CXX_COMPILER=g++ \ 
    -DCMAKE_TOOLCHAIN_FILE="/home/frucd/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build
cd ../..
echo "finished rebuilding web app files"
