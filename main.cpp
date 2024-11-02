#include <iostream>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "src/Map.hpp"

const std::string API_KEY = "7bcd6334-bc2e-4cbf-b9d4-61cb9e868869";

int main()
{
    Map map{};
    map.parse();

    std::string session_id;

    cpr::Response r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/session/start"},
                                cpr::Header{{"API-KEY", API_KEY}});
    if (r.status_code != 200)
    {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
        return EXIT_FAILURE;
    }
    session_id = r.text;
    std::cout << "SESSION CREATED\n";

    for (int day = 0; day <= 41; ++day)
    {
        nlohmann::json body;
        body["day"] = day;
        body["movements"] = nlohmann::json::array();

        nlohmann::json movement;
        movement["connectionId"] = "3fa85f64-5717-4562-b3fc-2c963f66afa6";
        movement["amount"] = 5;

        body["movements"].push_back(movement);

        r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/play/round"},
                      cpr::Header{{"API-KEY",      API_KEY},
                                  {"SESSION-ID",   session_id},
                                  {"Content-Type", "application/json"},

                      }, cpr::Body{to_string(body)});



        if (r.status_code != 200)
        {
            std::cout << r.status_code << '\n';
            std::cout << r.text << '\n';
            return EXIT_FAILURE;
        }
    }

    r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/session/end"},
                  cpr::Header{{"API-KEY", API_KEY}});
    if (r.status_code != 200)
    {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
        return EXIT_FAILURE;
    }
    std::cout << "SESSION CLOSED\n";

    return EXIT_SUCCESS;
}
