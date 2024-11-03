#include "DemandHeap.hpp"

DemandHeap::DemandHeap() {
    std::make_heap(demands.begin(), demands.end(), comparator);
}

void DemandHeap::addDemand(const Demand &d) {
    this->demands.push_back(d);
    std::push_heap(this->demands.begin(), this->demands.end(), comparator);
}

Demand DemandHeap::getRoot() {
    return this->demands.front();
}

void DemandHeap::extractRoot() {
    std::pop_heap(this->demands.begin(), this->demands.end(), comparator);
    this->demands.pop_back();
}

bool comparator(const Demand &a, const Demand &b) {
    if (a.endDay == b.endDay)
        // return a.late_penalty < b.late_penalty;
            return a.amount < b.amount; // asta e mai important ca sa n avem penalizari la final
    return a.endDay > b.endDay;
}


