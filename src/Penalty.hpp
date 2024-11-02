#pragma once

#include "string"
#include "unordered_map"
#include "iostream"
#include <algorithm>

class StaticPenalty
{
public:
    static std::unordered_map<std::string, float> errorMap;
    static void initializeErrorMap();

    static void printErrorMap() {
        std::vector<std::pair<std::string, int>> sortedErrors(errorMap.begin(), errorMap.end());

        std::sort(sortedErrors.begin(), sortedErrors.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        for (const auto& pair : errorMap) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }

    static void readPenaltyEnd();

    StaticPenalty() = delete;
};
