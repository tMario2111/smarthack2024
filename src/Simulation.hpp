#pragma once

#include <iostream>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "Map.hpp"
#include "Movements.hpp"

class Simulation
{
public:
    void run();

private:
    const std::string API_KEY = "7bcd6334-bc2e-4cbf-b9d4-61cb9e868869";

    Map map;

    int day;

    void updateRefineries();
};


