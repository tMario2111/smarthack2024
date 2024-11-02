//
// Created by Mario on 02-Nov-24.
//

#include "Round.hpp"

void Round::readRound(nlohmann::json json_read)
{
    if (json_read["round"].is_number_integer()) {
        round = json_read["round"];
    }
}