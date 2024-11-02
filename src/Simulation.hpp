#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "Map.hpp"
#include "Movements.hpp"
#include "Round.hpp"

class Simulation
{
public:
    void run();

private:
    const std::string API_KEY = "7bcd6334-bc2e-4cbf-b9d4-61cb9e868869";

    Map map;
    Movements movements;

    int day = 0;

    std::vector<nlohmann::json> json_movements;

    const float MINIMUM_TRANSPORT_CAPACITY = 0.5f;

    void resetTanks();
    void updateRefineries();
    void moveToTanks();
    void processMovements();
};


