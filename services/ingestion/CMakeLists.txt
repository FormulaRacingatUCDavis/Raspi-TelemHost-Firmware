cmake_minimum_required(VERSION 3.31)
project(Backend VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Backend
  src/main.cpp
  src/ingestion.cpp
)

target_include_directories(Backend PRIVATE include)

find_package(concurrentqueue CONFIG REQUIRED)
target_link_libraries(Backend PRIVATE concurrentqueue::concurrentqueue)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

target_include_directories(Backend PRIVATE ${MOSQUITTO_INCLUDE_DIRS})
target_link_libraries(Backend PRIVATE ${MOSQUITTO_LIBRARIES})