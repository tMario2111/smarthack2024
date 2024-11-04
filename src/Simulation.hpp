#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

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
//    const std::string API_KEY = "7bcd6334-bc2e-4cbf-b9d4-61cb9e868869";
    const std::string API_KEY = "8a1f8ab0-3af0-45b6-a86b-57de70718d41";
    const std::string IP = "https://smarthack2024-eval.cfapps.eu12.hana.ondemand.com/";

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

    static bool checkTankAlreadyInVector(const std::vector<std::vector<Tank*>>& vectorTank, Tank* tank);
};
