cmake_minimum_required(VERSION 3.28)
project(smarthack2024)

set(CMAKE_CXX_STANDARD 17)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-vincentlaucsb-csv-parser CONFIG REQUIRED)

add_executable(smarthack2024 main.cpp
        src/Map.cpp
        src/Map.hpp
        src/Movements.cpp
        src/Movements.hpp
        src/Simulation.cpp
        src/Simulation.hpp
        src/Round.cpp
        src/Round.hpp
        src/DemandHeap.cpp
        src/DemandHeap.hpp
        src/Penalty.cpp
        src/Penalty.hpp)

target_link_libraries(smarthack2024 PRIVATE cpr::cpr nlohmann_json::nlohmann_json
        unofficial::vincentlaucsb-csv-parser::csv)