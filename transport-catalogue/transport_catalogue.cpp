#include "transport_catalogue.h"

namespace guide
{
    using namespace stop_coordinate;

    void TransportCatalogue::AddStop(std::string name, Coordinates coordinates)
    {
        if (!stops_.count(name))
        {
            all_items_.push_back(name);
            stops_[all_items_.back()] = std::move(coordinates);
        }
    }

    void TransportCatalogue::AddBus(std::string name, std::vector<std::string_view> stops)
    {
        if (!buses_.count(name))
        {
            all_items_.push_back(name);
            for (const auto stop : stops)
            {
                auto it = std::find(all_items_.begin(), all_items_.end(), stop);
                buses_[all_items_.back()].push_back(*it);
                stops_and_buses_[*it].insert(all_items_.back());
            }
        }
    }

    BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const
    {
        int stops_on_route = 0;
        int unique_stops = 0;
        double route_length = 0.0;

        // Если маршрут не найден
        if (!buses_.count(name))
        {
            return {0, 0, 0};
        }

        stops_on_route = static_cast<int>(buses_.at(name).size());

        std::unordered_set<std::string_view> set_of_unique_stops;

        for (const auto stop : buses_.at(name))
        {
            set_of_unique_stops.insert(stop);
        }
        unique_stops = static_cast<int>(set_of_unique_stops.size());

        for (size_t i = 0; i < static_cast<size_t>(stops_on_route - 1); ++i)
        {
            route_length += ComputeDistance(stops_.at(buses_.at(name)[i]), stops_.at(buses_.at(name)[i + 1]));
        }
        return {stops_on_route, unique_stops, route_length};
    }

    std::set<std::string_view> TransportCatalogue::GetStopInfo(std::string_view name) const
    {
        std::set<std::string_view> buses;
        if (!stops_.count(name))
        {
            buses.insert("not found");
        }
        else
        {
            if (stops_and_buses_.count(name))
            {
                buses = stops_and_buses_.at(name);
            }
            else
            {
                buses.insert("no buses");
            }
        }

        return buses;
    }

    void TransportCatalogue::GetAllInfo() const
    {
        std ::cout << "---------------Stops-------------------" << std::endl;
        for (const auto &[stop, coordinate] : stops_)
        {
            std::cout << stop << " " << coordinate.lat << " " << coordinate.lng << std::endl;
        }
        std ::cout << "-----------------Buses-----------------" << std::endl;
        for (const auto &[number, stops] : buses_)
        {
            std::cout << number << std::endl;
            for (const auto stop : stops)
            {
                std::cout << stop << " - ";
            }
            std::cout << std::endl;
        }
        std ::cout << "-----------------Stops--and--Buses-----------------" << std::endl;
        for (const auto &[stop, buses] : stops_and_buses_)
        {
            std::cout << stop << std::endl;
            for (const auto bus : buses)
            {
                std::cout << bus << " - ";
            }
            std::cout << std::endl;
        }
        std ::cout << "---------------------------------" << std::endl;
    }
}
