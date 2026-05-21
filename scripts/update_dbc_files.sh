#!/bin/bash

cd /home/frucd/Raspi-TelemHost-Firmware/

echo "removing old dbc source files"
rm dashboard/src/cm200_db.c dashboard/inc/cm200_db.h dashboard/src/fe13_db.c dashboard/inc/fe13_db.h services/daq/src/cm200_db.c services/daq/inc/cm200_db.h services/daq/src/fe_13.c services/daq/inc/fe13_db.h

echo "generating new dbc source files"
python3 -m cantools generate_c_source --database-name cm200_db --output-directory dashboard/src resources/”20240129 Gen5 CAN DB.dbc”
python3 -m cantools generate_c_source --database-name cm200_db --output-directory services/daq/src resources/”20240129 Gen5 CAN DB.dbc”
python3 -m cantools generate_c_source --database-name fe13_db --output-directory dashboard/src resources/FE12.dbc && mv dashboard/src/*.h dashboard/inc
python3 -m cantools generate_c_source --database-name fe13_db --output-directory services/daq/src resources/FE12.dbc && mv services/daq/src/*.h services/daq/inc

echo "finished updating dbc source files"