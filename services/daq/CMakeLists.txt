cmake_minimum_required(VERSION 3.31)
project(DAQ VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_COMMON
  src/ingestion.cpp
  src/decode.cpp
  src/fe12_db.c
  src/cm200_db.c
)

add_executable(Backend src/publisher.cpp ${SRC_COMMON})
add_executable(Logger src/logger.cpp ${SRC_COMMON})

target_include_directories(Backend PRIVATE include)
target_include_directories(Logger PRIVATE include)

find_package(concurrentqueue CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

foreach(target IN ITEMS Backend Logger)
    target_link_libraries(${target} PRIVATE concurrentqueue::concurrentqueue)
    target_link_libraries(${target} PRIVATE nlohmann_json::nlohmann_json)
    target_include_directories(${target} PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
    target_link_libraries(${target} PRIVATE ${MOSQUITTO_LIBRARIES})
endforeach()