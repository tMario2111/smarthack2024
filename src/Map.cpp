#include "Map.hpp"

void Map::parse()
{
    {
        csv::CSVReader reader{"../map/refineries.csv"};
        for (auto &row: reader)
        {
            auto refinery = new Refinery;
            refinery->id = row["id"].get<std::string>();
            refinery->name = row["name"].get<std::string>();
            refinery->max_output = row["max_output"].get<float>();
            refinery->production = row["production"].get<float>();
            refinery->overflow_penalty = row["overflow_penalty"].get<float>();
            refinery->underflow_penalty = row["underflow_penalty"].get<float>();
            refinery->over_output_penalty = row["over_output_penalty"].get<float>();
            refinery->production_cost = row["production_cost"].get<float>();
            refinery->production_co2 = row["production_co2"].get<float>();
            refinery->stock = row["initial_stock"].get<float>();
            refinery->capacity = row["capacity"].get<float>();
            this->nodes[refinery->id] = refinery;
        }
    }

    {
        csv::CSVReader reader{"../map/tanks.csv"};
        for (auto &row: reader)
        {
            auto tank = new Tank;
            tank->id = row["id"].get<std::string>();
            tank->name = row["name"].get<std::string>();
            tank->capacity = row["capacity"].get<float>();
            tank->max_input = row["max_input"].get<float>();
            tank->max_output = row["max_output"].get<float>();
            tank->underflow_penalty = row["underflow_penalty"].get<float>();
            tank->overflow_penalty = row["overflow_penalty"].get<float>();
            tank->over_output_penalty = row["over_output_penalty"].get<float>();
            tank->over_input_penalty = row["over_input_penalty"].get<float>();
            tank->stock = row["initial_stock"].get<float>();
            this->nodes[tank->id] = tank;
        }
    }

    {
        csv::CSVReader reader{"../map/customers.csv"};
        for (auto &row: reader)
        {
            auto customer = new Customer;
            customer->id = row["id"].get<std::string>();
            customer->name = row["name"].get<std::string>();
            customer->max_input = row["max_input"].get<float>();
            customer->over_input_penalty = row["over_input_penalty"].get<float>();
            customer->late_delivery_penalty = row["late_delivery_penalty"].get<float>();
            customer->early_delivery_penalty = row["early_delivery_penalty"].get<float>();
            this->nodes[customer->id] = customer;
        }
    }

    {
        csv::CSVReader reader{"../map/connections.csv"};
        for (auto &row: reader)
        {
            Connection connection;
            connection.id = row["id"].get<std::string>();
            connection.distance = row["distance"].get<float>();
            connection.lead_time_days = row["lead_time_days"].get<int>();
            connection.connection_type = row["connection_type"].get<std::string>();
            connection.max_capacity = row["max_capacity"].get<float>();

            auto from = row["from_id"].get<std::string>();
            auto to = row["to_id"].get<std::string>();
            this->nodes[from]->neighbors.emplace_back(connection, this->nodes[to]);
        }
    }
}

void Map::print()
{
    for (auto &[_, val]: nodes)
    {
        std::cout << val->id.substr(0, 2) << " : ";
        for (auto &n: val->neighbors)
        {
            std::cout << n.second->id.substr(0, 2) << ' ';
        }
        std::cout << '\n';
    }
}
