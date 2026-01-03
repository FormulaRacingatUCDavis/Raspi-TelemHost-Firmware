cmake_minimum_required(VERSION 3.31)
project(DAQ VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DAQ
  src/main.cpp
  src/ingestion.cpp
  src/decode.cpp
  src/fe12_db.c
  src/cm200_db.c
)

target_include_directories(DAQ PRIVATE include)

find_package(concurrentqueue CONFIG REQUIRED)
target_link_libraries(DAQ PRIVATE concurrentqueue::concurrentqueue)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(DAQ PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(DAQ PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
target_link_libraries(DAQ PRIVATE ${MOSQUITTO_LIBRARIES})