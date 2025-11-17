cmake_minimum_required(VERSION 3.24)



project(RaspiTelemHost C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
include(cmake/deps.cmake)

add_subdirectory(DAQ/Drivers/ADS1263)

set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "/home/frucd/Projects/Qt/6.10.0/gcc_arm64")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)

# Collect all DAQ sources
set(DAQ_SOURCES
    DAQ/Src/main.cpp
    DAQ/Src/telemetry.cpp
    DAQ/Src/config.cpp
    DAQ/Src/observer.cpp
    DAQ/Src/variables.cpp

    DAQ/Inc/variables.h
    DAQ/Inc/logthread.h
)

# use the qrc files to add all the qml files to be findable and able to call it 
set(DASHBOARD_RESOURCES dashboard.qrc)

# the normal cmake executable but wrapped in qt
qt_add_executable(RaspiTelemHost ${DAQ_SOURCES} ${DASHBOARD_RESOURCES})

# set_target_properties(RaspiTelemHost PROPERTIES
set_target_properties(RaspiTelemHost PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(RaspiTelemHost PUBLIC  
    DAQ/Inc
    DAQ/ui
)

target_link_libraries(RaspiTelemHost PRIVATE
    ADS1263
    bcm2835
    m
    dbcppp::dbcppp
    nlohmann_json::nlohmann_json
    Qt6::Core # all four are needed for the dashbaord
    Qt6::Gui 
    Qt6::Quick 
    Qt6::Qml
)

target_compile_definitions(RaspiTelemHost PRIVATE
    RPI
    USE_BCM2835_LIB
    DEBUG
)

add_custom_command(TARGET RaspiTelemHost POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/DAQ/config.json
        $<TARGET_FILE_DIR:RaspiTelemHost>/DAQ/config.json
)

# adds the dbc file into the build so that the stuff in telemetry.cpp can find it
add_custom_command(TARGET RaspiTelemHost POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/DAQ/20240129 Gen5 CAN DB.dbc" # needs to be in quotes since file has spaces in it
        "$<TARGET_FILE_DIR:RaspiTelemHost>/DAQ/20240129 Gen5 CAN DB.dbc"
)

add_custom_command(TARGET RaspiTelemHost POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/DAQ/FE12.dbc
        $<TARGET_FILE_DIR:RaspiTelemHost>/DAQ/FE12.dbc
)
