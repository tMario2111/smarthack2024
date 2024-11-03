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
};
