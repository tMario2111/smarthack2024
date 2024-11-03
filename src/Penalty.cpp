#include "Penalty.hpp"

std::unordered_map<std::string, int> StaticPenalty::errorMap;

void StaticPenalty::initializeErrorMap() {
    errorMap["INVALID_CONNECTION"] = 0;
    errorMap["REFINERY_OVER_OUTPUT"] = 0;
    errorMap["STORAGE_TANK_OVER_OUTPUT"] = 0;
    errorMap["STORAGE_TANK_OVER_INPUT"] = 0;
    errorMap["CUSTOMER_OVER_INPUT"] = 0;
    errorMap["CONNECTION_OVER_CAPACITY"] = 0;
    errorMap["CUSTOMER_UNEXPECTED_DELIVERY"] = 0;
    errorMap["CUSTOMER_EARLY_DELIVERY"] = 0;
    errorMap["CUSTOMER_LATE_DELIVERY"] = 0;
    errorMap["REFINERY_OVERFLOW"] = 0;
    errorMap["STORAGE_TANK_OVERFLOW"] = 0;
    errorMap["REFINERY_UNDERFLOW"] = 0;
    errorMap["STORAGE_TANK_UNDERFLOW"] = 0;
    errorMap["PENDING_MOVEMENTS"] = 0;
    errorMap["UNMET_DEMANDS"] = 0;
}
