#pragma once

#include <string>
#include "vector"
#include "unordered_map"
#include <nlohmann/json.hpp>
#include "iostream"

#include "Map.hpp"
#include "Penalty.hpp"

struct Demand
{
    std::string customerId;
    int amount;
    int postDay;
    int startDay;
    int endDay;
    float late_penalty;
    float early_penalty;
};

enum Penalties
{
    INVALID_CONNECTION,
    REFINERY_OVER_OUTPUT,
    STORAGE_TANK_OVER_OUTPUT,
    STORAGE_TANK_OVER_INPUT,
    CUSTOMER_OVER_INPUT,
    CONNECTION_OVER_CAPACITY,
    CUSTOMER_UNEXPECTED_DELIVERY,
    CUSTOMER_EARLY_DELIVERY,
    CUSTOMER_LATE_DELIVERY,
    REFINERY_OVERFLOW,
    STORAGE_TANK_OVERFLOW,
    REFINERY_UNDERFLOW,
    STORAGE_TANK_UNDERFLOW,
    PENDING_MOVEMENTS,
    UNMET_DEMANDS
};




struct Penalty
{
    int day;
    Penalties type;
    std::string message;
    float cost;
    float co2;
};

struct Kpi
{
    int day;
    float cost;
    float co2;
    Kpi& operator=(const Kpi& other);
};

class Round
{
    Map& map;
    int round;
    std::vector<Penalty> penalties;
    Kpi deltaKpis;
    Kpi totalKpis;
public:
    Round(Map& map);

    std::vector<Demand> demands;
    Kpi finalKpi;
    void readRound(nlohmann::json json_read);
    void printRound();
    static Penalties stringToEnum(const std::string& str);
};
