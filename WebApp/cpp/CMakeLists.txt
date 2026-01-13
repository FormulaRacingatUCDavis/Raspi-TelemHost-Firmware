cmake_minimum_required(VERSION 3.31)

project(
    Backend
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(Backend)

add_subdirectory(Source)
target_include_directories(Backend PUBLIC Include)