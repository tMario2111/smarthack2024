#pragma once

#include <vector>
#include <algorithm>
#include <queue>
#include <string>

#include "Map.hpp"

struct Payload
{
    int arrival_day;
    float quanitity;
    std::string destination_id;
    Connection* connection;
};

bool comparator(const Payload &a, const Payload &b);

class Movements
{
private:
    std::vector<Payload> heap;

public:
    Movements();

    void addMovement(const Payload& p);
    int getRootDay();
    Payload extractRoot();
    size_t getSize();
};
