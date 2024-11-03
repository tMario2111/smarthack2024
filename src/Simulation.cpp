#include "Simulation.hpp"

void Simulation::run() {
    map.parse();

    std::string session_id;

    cpr::Response r = cpr::Post(cpr::Url{IP+":8080/api/v1/session/start"},
                                cpr::Header{{"API-KEY", API_KEY}});
    if (r.status_code != 200) {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
        exit(EXIT_FAILURE);
    }
    session_id = r.text;
    std::cout << "SESSION CREATED\n";

    Kpi final_kpi{};

    // Initialize penalty
    StaticPenalty::initializeErrorMap();

    Round round{this->map};

    for (day = 0; day <= 41; ++day) {

        this->processMovements();
        this->moveToTanks();

        nlohmann::json body;
        body["day"] = day;
        body["movements"] = nlohmann::json::array();

        for (auto &json_movement: this->json_movements)
            body["movements"].push_back(json_movement);

        // std::cout << body.dump(4) << "\n\n";

        r = cpr::Post(cpr::Url{IP+":8080/api/v1/play/round"},
                      cpr::Header{
                          {"API-KEY", API_KEY},
                          {"SESSION-ID", session_id},
                          {"Content-Type", "application/json"},

                      }, cpr::Body{to_string(body)});
        if (r.status_code != 200) {
            std::cout << r.status_code << '\n';
            std::cout << r.text << '\n';
            exit(EXIT_FAILURE);
        }
        this->json_movements.clear();

        auto json_response = nlohmann::json::parse(r.text);

        round.readRound(json_response);
        for (auto &d: round.demands)
            this->demands_heap.addDemand(d);

        //        if (json_response["round"] == 1)
        //            std::cout << json_response.dump(4) << "\n####\n";

        this->startFromDemand();
        this->updateRefineries();
    }

    r = cpr::Post(cpr::Url{IP+":8080/api/v1/session/end"},
                  cpr::Header{{"API-KEY", API_KEY}});

    if (r.status_code != 200) {
        std::cout << r.status_code << '\n';
        std::cout << r.text << '\n';
        exit(EXIT_FAILURE);
    }

    auto end_response = nlohmann::json::parse(r.text);
    round.readRound(end_response);


    final_kpi = round.finalKpi;
    StaticPenalty::printErrorMap();
    // Round::printDemandStats();

    std::cout << "CO2: " << final_kpi.co2 << '\n';
    std::cout << "COST: " << final_kpi.cost << '\n';

    std::cout << "SESSION CLOSED\n";
}

void Simulation::updateRefineries() {
    for (auto &[id, node]: this->map.nodes) {
        if (auto *refinery = dynamic_cast<Refinery *>(node)) {
            refinery->stock += refinery->production;
            if (refinery->stock > refinery->capacity)
                refinery->stock = refinery->capacity;
        }
    }
}


void Simulation::moveToTanks() {
    // Trebuie sa mutam SI DIN TANK IN TANK
    std::vector<std::vector<Tank>>

    for (auto &[id, node]: this->map.nodes) {
        if (auto *refinery = dynamic_cast<Refinery *>(node)) {
            float current_output = refinery->max_output;
            for (auto &neighbor: refinery->neighbors) {
                if (auto tank = dynamic_cast<Tank *>(neighbor.second)) {
                    if (tank->expected_stock >= tank->capacity)
                        continue;

                    auto remaining_tank_capacity = tank->capacity - tank->expected_stock;
                    auto quantity = std::min(
                        std::min(std::min(remaining_tank_capacity, neighbor.first.remaining_capacity),
                                 refinery->stock), current_output);

                    if (quantity > 0.f && quantity >= neighbor.first.max_capacity * MINIMUM_TRANSPORT_CAPACITY) {
                        refinery->stock -= quantity;
                        current_output -= quantity;
                        neighbor.first.remaining_capacity -= quantity;
                        tank->expected_stock = tank->stock + quantity;

                        Payload p;
                        p.quantity = quantity;
                        p.arrival_day = this->day + neighbor.first.lead_time_days;
                        p.destination_id = neighbor.second->id;
                        p.connection = &neighbor.first;
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

void Simulation::startFromDemand() {
    while (!this->demands_heap.demands.empty()) {
        auto demand = this->demands_heap.getRoot();
        auto customer = dynamic_cast<Customer *>(this->map.nodes[demand.customerId]);

        // make sorted list of Tank neighbors sorted by lead time
        std::vector<std::pair<Connection, Node *>> sorted_tanks;
        for (const auto& neighbor: customer->neighbors) {
            if (auto tank = dynamic_cast<Tank *>(neighbor.second)) {
                sorted_tanks.push_back(neighbor);
            }
        }
        std::sort(sorted_tanks.begin(), sorted_tanks.end(), [](const auto &a, const auto &b) {
            return a.first.lead_time_days < b.first.lead_time_days;
        });

        bool ajunge = false;
        for (auto neighbor: sorted_tanks) {
            if (auto tank = dynamic_cast<Tank *>(neighbor.second)) {
                if (demand.amount == 0) {
                    std::cout << "AM AJUNS AICI";
                    break;
                }
                auto quantity = std::min(static_cast<float>(demand.amount), tank->stock);
                // if (quantity == 0) {
                //     ok = false;
                //     break;
                // }

                demand.amount -= quantity;
                tank->stock -= quantity;
                tank->expected_stock -= quantity;


                if (demand.amount == 0) {
                    ajunge = true;
                    break;
                }

                Payload p;
                p.quantity = quantity;
                p.arrival_day = this->day + neighbor.first.lead_time_days;
                p.destination_id = neighbor.second->id;
                p.connection = &neighbor.first;
                p.connection->remaining_capacity -= p.quantity;

                movements.addMovement(p);

                nlohmann::json json_movement;
                json_movement["connectionId"] = neighbor.first.id;
                json_movement["amount"] = quantity;
                this->json_movements.push_back(json_movement);
            }
        }
        if (ajunge) {
            this->demands_heap.extractRoot();
        }
        else {
            break;
        }
    }
}



void Simulation::processMovements() {
    while (this->movements.getSize() > 0 && this->movements.getRootDay() == this->day) {
        auto p = this->movements.extractRoot();
        auto node = this->map.nodes[p.destination_id];
        if (auto tank = dynamic_cast<Tank *>(node)) {
            tank->stock = tank->expected_stock;
            p.connection->remaining_capacity += p.quantity;
        } else if (auto customer = dynamic_cast<Customer *>(node)) {
            p.connection->remaining_capacity += p.quantity;
            std::cout << "S-a ajuns la customer\n";
        }
    }
}


