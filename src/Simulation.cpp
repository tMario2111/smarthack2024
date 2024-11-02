#include "Simulation.hpp"

void Simulation::run()
{
    map.parse();

    std::string session_id;

    cpr::Response r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/session/start"},
                                cpr::Header{{"API-KEY", API_KEY}});
    if (r.status_code != 200)
    {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
        exit(EXIT_FAILURE);
    }
    session_id = r.text;
    std::cout << "SESSION CREATED\n";

    for (day = 0; day <= 41; ++day)
    {
        this->updateRefineries();

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
            exit(EXIT_FAILURE);
        }

        auto json_response = nlohmann::json(r.text);

    }

    r = cpr::Post(cpr::Url{"192.168.123.221:8080/api/v1/session/end"},
                  cpr::Header{{"API-KEY", API_KEY}});
    if (r.status_code != 200)
    {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
        exit(EXIT_FAILURE);
    }
    std::cout << "SESSION CLOSED\n";
}

void Simulation::updateRefineries()
{
    if (day == 0)
        return;

    for (auto& [id, node] : this->map.nodes)
    {
        if (auto* refinery = dynamic_cast<Refinery*>(node))
        {
            refinery->stock += refinery->production;
            if (refinery->stock > refinery->capacity)
                refinery->stock = refinery->capacity;
        }
    }
}
