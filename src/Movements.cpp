#include "Movements.hpp"

Movements::Movements()
{
    std::make_heap(this->heap.begin(), this->heap.end(), comparator);
}

void Movements::addMovement(const Payload& p)
{
    this->heap.push_back(p);
    std::push_heap(this->heap.begin(), this->heap.end(), comparator);
}

int Movements::getRootDay()
{
    return this->heap.front().arrival_day;
}

Payload Movements::extractRoot()
{
    auto p = this->heap.front();
    std::pop_heap(this->heap.begin(), this->heap.end(), comparator);
    this->heap.pop_back();
    return p;
}

size_t Movements::getSize()
{
    return this->heap.size();
}

bool comparator(const Payload &a, const Payload &b)
{
    return a.arrival_day > b.arrival_day;
}
