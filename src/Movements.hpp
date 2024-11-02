#pragma once

#include <vector>
#include <algorithm>
#include <queue>
#include <string>

struct Payload
{
    int arrival_day;
    float quanitity;
    std::string destination_id;
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
