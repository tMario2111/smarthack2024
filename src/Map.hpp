#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#include <csv.hpp>

class Connection {
public:
    std::string id;
    float distance;
    int lead_time_days;
    std::string connection_type;
    float max_capacity;
    float remaining_capacity;
};

class Node {
public:
    virtual ~Node() = default;

    std::string id, name;
    std::vector<std::pair<Connection, Node *> > neighbors;
};

class Refinery : public Node {
public:
    float capacity;
    float max_output;
    float production;
    float overflow_penalty;
    float underflow_penalty;
    float over_output_penalty;
    float production_cost;
    float production_co2;
    float stock;
};

class Tank : public Node {
public:
    float capacity;
    float max_input;
    float max_output;
    float overflow_penalty;
    float underflow_penalty;
    float over_input_penalty;
    float over_output_penalty;
    float stock;

    float expected_stock;
    float remaining_input;
};

class Customer : public Node {
public:
    float max_input;
    float over_input_penalty;
    float late_delivery_penalty;
    float early_delivery_penalty;
};

class Map {
public:
    void parse();

    void print();

    std::unordered_map<std::string, Node *> nodes;
};
