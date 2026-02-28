cmake_minimum_required(VERSION 3.25)

project(DAQ LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DAQ
    src/main.cpp
    src/daqhelper.cpp
    src/fe12_db.c
    src/cm200_db.c
)

target_include_directories(DAQ PRIVATE include)

find_package(concurrentqueue CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

target_link_libraries(DAQ PRIVATE
    concurrentqueue::concurrentqueue
    nlohmann_json::nlohmann_json
    ${MOSQUITTO_LIBRARIES}
)

target_include_directories(DAQ PRIVATE
    ${MOSQUITTO_INCLUDE_DIRS}
)