cmake_minimum_required(VERSION 3.28)
project(smarthack2024)

set(CMAKE_CXX_STANDARD 17)

find_package(cpr CONFIG REQUIRED)

add_executable(smarthack2024 main.cpp)

target_link_libraries(smarthack2024 PRIVATE cpr::cpr)