#pragma once

#include "string"
#include "unordered_map"
#include "iostream"
#include <algorithm>
#include <vector>

class StaticPenalty {
public:
    static std::unordered_map<std::string, int> errorMap;

    static void initializeErrorMap();

    static void printErrorMap() {
        std::vector<std::pair<std::string, int> > sortedErrors(errorMap.begin(), errorMap.end());

        std::sort(sortedErrors.begin(), sortedErrors.end(), [](const auto &a, const auto &b) {
            return a.second > b.second;
        });

        for (const auto &pair: errorMap) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }

    static int sumPenalty() {
        int sum = 0;
        for (const auto &pair: errorMap) {
            sum += pair.second;
        }
        return sum;
    }

    StaticPenalty() = delete;
};
