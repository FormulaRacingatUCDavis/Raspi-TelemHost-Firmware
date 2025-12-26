cmake_minimum_required(VERSION 3.31)
project(Backend VERSION 1.0 LANGUAGES C CXX)

add_executable(Backend
  src/main.cpp
  src/ingestion.cpp
  src/logger.cpp
)

target_include_directories(Backend PRIVATE include)