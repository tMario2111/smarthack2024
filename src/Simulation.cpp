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
        this->moveToTanks(day);

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
        round.printRoundCost();
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

    std::cout << "TOTAL PENALTY: " << StaticPenalty::sumPenalty() << '\n';
    std::cout << "CO2 + COST - PENALTY: " << (int) (final_kpi.co2 + final_kpi.cost - StaticPenalty::sumPenalty()) <<
            '\n';

    //Calcularea sumei costului si co2 pentru TOATE rafinariile:
    float total_cost = 0;
    float total_co2 = 0;
    for (auto &[id, node]: this->map.nodes) {
        if (auto *refinery = dynamic_cast<Refinery *>(node)) {
            total_cost += refinery->cost;
            total_co2 += refinery->co2;
        }
    }
    // Afisarea
    std::cout << "TOTAL COST RAFINARII: " << total_cost << '\n';
    std::cout << "TOTAL CO2 RAFINARII: " << total_co2 << '\n';

    std::cout << "SESSION CLOSED\n";
}

void Simulation::updateRefineries() {
    for (auto &[id, node]: this->map.nodes) {
        if (auto *refinery = dynamic_cast<Refinery *>(node)) {
            refinery->stock += refinery->production;
            if (refinery->stock > refinery->capacity)
                refinery->stock = refinery->capacity;
            refinery->cost += refinery->production_cost;
            refinery->co2 += refinery->production_co2;
        }
    }
}


void Simulation::moveToTanks(int round) {
    // Trebuie sa mutam SI DIN TANK IN TANK
    std::vector<std::vector<Tank*>> vectorTank; // vector de vectori de tankuri

    // pentru fiecare rafinarie luam tank-ul si il punem pe nivelul 1! de tankuri
    std::vector<Tank*> nivel1;
    for (auto &[id, node]: this->map.nodes) {
        if (auto *refinery = dynamic_cast<Refinery *>(node)) {
            float current_output = refinery->max_output;
            // Sort by lead time * distance
            std::sort(refinery->neighbors.begin(), refinery->neighbors.end(), [](const auto &a, const auto &b) {
                return a.first.distance * a.first.lead_time_days < b.first.distance * b.first.lead_time_days;
            });

            for (auto &neighbor: refinery->neighbors) {
                if (auto tank = dynamic_cast<Tank *>(neighbor.second)) {
                    nivel1.push_back(tank);
                    if (tank->expected_stock >= tank->capacity)
                        continue;

                    auto remaining_tank_capacity = tank->capacity - tank->expected_stock;
                    auto quantity = std::min(remaining_tank_capacity, current_output);

                    if ((quantity / (neighbor.first.distance * neighbor.first.lead_time_days)) > 4.0) continue;


                    if (quantity > 0.f && quantity >= neighbor.first.max_capacity * MINIMUM_TRANSPORT_CAPACITY && 42 - round >= neighbor.first.lead_time_days) {
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

    vectorTank.push_back(nivel1);

    // pentru fiecare nivel de tankuri facem mutarile
    auto exista_tank = true;
    auto index = 1;
    while (exista_tank) {
        exista_tank = false;
        std::vector<Tank*> nivel;
        for (auto tank: vectorTank[index-1]) {
            for (auto neighbor: tank->neighbors) {
                if (auto tank2 = dynamic_cast<Tank *>(neighbor.second)) {
                    if (chechTankAlreadyInVector(vectorTank, tank2))
                        continue;
                    nivel.push_back(tank2);
                    exista_tank = true;
                    if (tank2->expected_stock >= tank2->capacity)
                        continue;


                    auto remaining_tank_capacity = tank2->capacity - tank2->expected_stock;
                    auto quantity = std::min(remaining_tank_capacity, tank->stock);

                    // if (quantity < 1000000) continue;
                    // magic number!
                    if (quantity / (neighbor.first.distance * neighbor.first.lead_time_days) > .5 ) continue;

                    std::cout << "Quantity: " << quantity / (neighbor.first.distance * neighbor.first.lead_time_days) << '\n';


                    if (quantity > 0.f && quantity >= neighbor.first.max_capacity * MINIMUM_TRANSPORT_CAPACITY && 42 - round >= neighbor.first.lead_time_days) {
                        tank->expected_stock -= quantity;
                        neighbor.first.remaining_capacity -= quantity;
                        tank2->expected_stock = tank2->stock + quantity;

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
        vectorTank.push_back(nivel);
        index++;
    }
}

void Simulation::startFromDemand() {
    bool ok = true;
    while (ok && !this->demands_heap.demands.empty()) {
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
            return a.first.distance < b.first.distance;
        });

        bool de_scos = false;
        for (auto neighbor: sorted_tanks) {
            if (auto tank = dynamic_cast<Tank *>(neighbor.second)) {
                if (tank->stock <= 0.f)
                    continue;
                // std::cout << "Am pus ceva aici";

                auto quantity = std::min((demand.amount), tank->stock);

                demand.amount -= quantity;
                tank->stock -= quantity;
                tank->expected_stock -= quantity;




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

                if (std::fpclassify(demand.amount) == FP_ZERO) {
                    de_scos = true;
                    break;
                }

            }
        }
        if(de_scos)
            this->demands_heap.extractRoot();
        else
            ok = false;

    }
}

bool Simulation::chechTankAlreadyInVector(const std::vector<std::vector<Tank *>>& vectorTank, Tank *tank) {
    for (auto &vector: vectorTank) {
        for (auto &tank2: vector) {
            if (tank->id == tank2->id)
                return true;
        }
    }
    return false;
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


