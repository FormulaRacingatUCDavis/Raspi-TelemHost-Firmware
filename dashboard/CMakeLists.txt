cmake_minimum_required(VERSION 3.24)

project(driver_dashboard C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(deps.cmake)

set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "/home/frucd/Projects/Qt/6.10.0/gcc_arm64")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)

set(DAQ_SOURCES
    src/main.cpp
    src/daq.cpp
    src/variables.cpp
    src/fe13_db.c
    src/cm200_db.c

    inc/variables.h
    inc/logthread.h
)

set(DASHBOARD_RESOURCES dashboard.qrc)

qt_add_executable(driver_dashboard ${DAQ_SOURCES} ${DASHBOARD_RESOURCES})

target_include_directories(driver_dashboard PUBLIC  
    inc
    ui
)

target_link_libraries(driver_dashboard PRIVATE
    m
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
)