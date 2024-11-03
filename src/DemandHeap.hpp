#pragma once

#include <vector>
#include <algorithm>
#include <queue>
#include <string>

#include "Round.hpp"

bool comparator(const Demand &a, const Demand &b);

class DemandHeap {
public:
    DemandHeap();

    void addDemand(const Demand &d);

    Demand getRoot();

    void extractRoot();

    std::vector<Demand> demands;

    void printHeap() {
        for (const auto &demand: demands) {
            std::cout << "Customer ID: " << demand.customerId << std::endl;
            std::cout << "Amount: " << demand.amount << std::endl;
            std::cout << "Post Day: " << demand.postDay << std::endl;
            std::cout << "Start Day: " << demand.startDay << std::endl;
            std::cout << "End Day: " << demand.endDay << std::endl;
        }
    }
};
