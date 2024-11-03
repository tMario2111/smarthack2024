#include "Round.hpp"

std::unordered_map<int, int> Round::demandStats;


void Round::readRound(nlohmann::json json_read) {
    demands.clear();

    if (json_read["round"].is_number_integer()) {
        round = json_read["round"];
    } else {
        round = static_cast<int>(json_read["round"]);
    }

    if (round != 42) {
        for (const auto &demand_read: json_read["demand"]) {
            Demand demand;
            demand.amount = demand_read["amount"];
            demand.postDay = demand_read["postDay"];
            demand.endDay = demand_read["endDay"];
            demand.startDay = demand_read["startDay"];
            demand.customerId = demand_read["customerId"];
            if (Customer *customer = dynamic_cast<Customer *>(map.nodes[demand.customerId])) {
                demand.early_penalty = customer->early_delivery_penalty;
                demand.late_penalty = customer->late_delivery_penalty;
            }
            demands.push_back(demand);
        }
    }
    for (const auto &penalty_read: json_read["penalties"]) {
        Penalty penalty;
        penalty.day = penalty_read["day"];
        penalty.message = penalty_read["message"];
        penalty.cost = penalty_read["cost"];
        penalty.co2 = penalty_read["co2"];
        penalty.type = stringToEnum(penalty_read["type"]);
        StaticPenalty::errorMap[penalty_read["type"]] += penalty.cost + penalty.co2;


        if (round != 42) {
            nlohmann::json deltaKpi;
            deltaKpi = json_read["deltaKpis"];

            deltaKpis.co2 = deltaKpi["co2"];
            deltaKpis.day = deltaKpi["day"];
            deltaKpis.cost = deltaKpi["cost"];
        }
    }

    nlohmann::json totalKpi;
    totalKpi = json_read["totalKpis"];

    totalKpis.co2 = totalKpi["co2"];
    totalKpis.day = totalKpi["day"];
    totalKpis.cost = totalKpi["cost"];

    if (round == 42)
        finalKpi = totalKpis;

    demandsStats();
}

void Round::printRound() {
    std::cout << "Round: " << round << std::endl;

    std::cout << "Demands:" << std::endl;
    for (const auto &demand: demands) {
        std::cout << "  Customer ID: " << demand.customerId << std::endl;
        std::cout << "  Amount: " << demand.amount << std::endl;
        std::cout << "  Post Day: " << demand.postDay << std::endl;
        std::cout << "  Start Day: " << demand.startDay << std::endl;
        std::cout << "  End Day: " << demand.endDay << std::endl;
    }

    std::cout << "Penalties:" << std::endl;
    for (const auto &penalty: penalties) {
        std::cout << "  Day: " << penalty.day << std::endl;
        std::cout << "  Type: " << penalty.type << std::endl;
        std::cout << "  Message: " << penalty.message << std::endl;
        std::cout << "  Cost: " << penalty.cost << std::endl;
        std::cout << "  CO2: " << penalty.co2 << std::endl;
    }

    std::cout << "Delta KPIs:" << std::endl;
    std::cout << "  Day: " << deltaKpis.day << std::endl;
    std::cout << "  Cost: " << deltaKpis.cost << std::endl;
    std::cout << "  CO2: " << deltaKpis.co2 << std::endl;

    std::cout << "Total KPIs:" << std::endl;
    std::cout << "  Day: " << totalKpis.day << std::endl;
    std::cout << "  Cost: " << totalKpis.cost << std::endl;
    std::cout << "  CO2: " << totalKpis.co2 << std::endl;
}

Penalties Round::stringToEnum(const std::string &str) {
    static const std::unordered_map<std::string, Penalties> stringToEnumMap = {
        {"INVALID_CONNECTION", INVALID_CONNECTION},
        {"REFINERY_OVER_OUTPUT", REFINERY_OVER_OUTPUT},
        {"STORAGE_TANK_OVER_OUTPUT", STORAGE_TANK_OVER_OUTPUT},
        {"STORAGE_TANK_OVER_INPUT", STORAGE_TANK_OVER_INPUT},
        {"CUSTOMER_OVER_INPUT", CUSTOMER_OVER_INPUT},
        {"CONNECTION_OVER_CAPACITY", CONNECTION_OVER_CAPACITY},
        {"CUSTOMER_UNEXPECTED_DELIVERY", CUSTOMER_UNEXPECTED_DELIVERY},
        {"CUSTOMER_EARLY_DELIVERY", CUSTOMER_EARLY_DELIVERY},
        {"CUSTOMER_LATE_DELIVERY", CUSTOMER_LATE_DELIVERY},
        {"REFINERY_OVERFLOW", REFINERY_OVERFLOW},
        {"STORAGE_TANK_OVERFLOW", STORAGE_TANK_OVERFLOW},
        {"REFINERY_UNDERFLOW", REFINERY_UNDERFLOW},
        {"STORAGE_TANK_UNDERFLOW", STORAGE_TANK_UNDERFLOW},
        {"PENDING_MOVEMENTS", PENDING_MOVEMENTS},
        {"UNMET_DEMANDS", UNMET_DEMANDS}
    };

    auto it = stringToEnumMap.find(str);
    if (it != stringToEnumMap.end()) {
        return it->second;
    } else {
        throw std::invalid_argument("Invalid string for enum conversion: " + str);
    }
}

Round::Round(Map &map) : map{map} {
}

Kpi &Kpi::operator=(const Kpi &other) {
    this->cost = other.cost;
    this->co2 = other.co2;
    this->day = other.day;

    return *this;
}
