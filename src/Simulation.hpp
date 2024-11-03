#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "Map.hpp"
#include "Movements.hpp"
#include "Round.hpp"
#include "DemandHeap.hpp"
#include "Penalty.hpp"

class Simulation {
public:
    void run();

private:
    const std::string API_KEY = "7bcd6334-bc2e-4cbf-b9d4-61cb9e868869";
    const std::string IP = "192.168.123.221";

    Map map;
    Movements movements;
    DemandHeap demands_heap;

    int day = 0;

    std::vector<nlohmann::json> json_movements;

    const float MINIMUM_TRANSPORT_CAPACITY = 0.5f;
    const float REFINERY_MOVE_WEIGTH = 8.f;

    void updateRefineries();

    void moveToTanks(int round);

    void processMovements();

    void startFromDemand();

    static bool chechTankAlreadyInVector(const std::vector<std::vector<Tank*>>& vectorTank, Tank* tank);
};
