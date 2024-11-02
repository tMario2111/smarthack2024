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
        this->processMovements();
        this->moveToTanks();

        nlohmann::json body;
        body["day"] = day;
        body["movements"] = nlohmann::json::array();

//        nlohmann::json movement;
//        movement["connectionId"] = "3fa85f64-5717-4562-b3fc-2c963f66afa6";
//        movement["amount"] = 5;

        for (auto& json_movement : this->json_movements)
            body["movements"].push_back(json_movement);

        std::cout << body.dump(4) << "\n\n";

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
        this->json_movements.clear();

        auto json_response = nlohmann::json(r.text);

        this->updateRefineries();
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
    for (auto &[id, node]: this->map.nodes)
    {
        if (auto *refinery = dynamic_cast<Refinery *>(node))
        {
            refinery->stock += refinery->production;
            if (refinery->stock > refinery->capacity)
                refinery->stock = refinery->capacity;
        }
    }
}

void Simulation::moveToTanks()
{
    for (auto &[id, node]: this->map.nodes)
    {
        if (auto *refinery = dynamic_cast<Refinery *>(node))
        {
            for (auto &neighbor: refinery->neighbors)
            {
                if (auto tank = dynamic_cast<Tank *>(neighbor.second))
                {
                    if (tank->expected_stock >= tank->capacity)
                        continue;

                    auto remaining_tank_capacity = tank->capacity - tank->expected_stock;
                    auto quantity = std::min(std::min(remaining_tank_capacity, neighbor.first.max_capacity),
                                             refinery->stock);
                    if (quantity > 0.f && quantity >= neighbor.first.max_capacity * MINIMUM_TRANSPORT_CAPACITY)
                    {
                        refinery->stock -= quantity;
                        tank->expected_stock = tank->stock + quantity;

                        Payload p;
                        p.quanitity = quantity;
                        p.arrival_day = this->day + neighbor.first.lead_time_days;
                        p.destination_id = neighbor.second->id;
                        movements.addMovement(p);

                        nlohmann::json json_movement;

                        json_movement["connectionId"] = neighbor.first.id;
                        json_movement["amount"] = quantity;
                        this->json_movements.push_back(json_movement);
                    }
                }
            }
        }
    }
}

void Simulation::processMovements()
{
    while (this->movements.getSize() > 0 && this->movements.getRootDay() == this->day)
    {
        auto p = this->movements.extractRoot();
        auto node = this->map.nodes[p.destination_id];
        if (auto tank = dynamic_cast<Tank *>(node))
        {
            tank->stock = tank->expected_stock;
        } else if (auto customer = dynamic_cast<Customer *>(node))
        {
            // Momentan
            continue;
        }
    }
}
