#include <iostream>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "src/Map.hpp"

const std::string API_KEY = "7bcd6334-bc2e-4cbf-b9d4-61cb9e868869";

int main()
{
    std::string session_id;

    cpr::Response r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/session/start"},
                                cpr::Header{{"API-KEY", API_KEY}});
    if (r.status_code != 200)
    {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
    }
    session_id = r.text;

    r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/play/round"},
                  cpr::Header{{"API-KEY",      API_KEY},
                              {"SESSION-ID",   session_id},
                              {"Content-Type", "application/json"},
                  },
                  cpr::Body{"{\n"
                            "  \"day\": 0,\n"
                            "  \"movements\": [\n"
                            "    {\n"
                            "      \"connectionId\": \"3fa85f64-5717-4562-b3fc-2c963f66afa6\",\n"
                            "      \"amount\": 0\n"
                            "    }\n"
                            "  ]\n"
                            "}"});
    if (r.status_code != 200)
    {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
    }

    r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/session/end"},
                  cpr::Header{{"API-KEY", API_KEY}});
    if (r.status_code != 200)
    {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
    }
    std::cout << "SESSION CREATED AND CLOSED SUCCESSFULLY\n";

    Map map{};
    map.parse();
    map.print();

    return 0;
}
