#include "request_handler.h"
#include "transport_catalogue.h"
#include "json.h"

#include <sstream>

namespace guide
{
    json::Node ParseStat(const TransportCatalogue &transport_catalogue, map_renderer::MapRenderer &map_renderer, int id, std::string request_name, std::string name)
    {
        json::Array buses;
        json::Dict answers_info;
        if (request_name == "Bus")
        {

            BusInfo bus_info = transport_catalogue.GetBusInfo(name);

            if (bus_info.stops_on_route == 0)
            {
                answers_info.insert({"request_id", id});
                answers_info.insert({"error_message", std::string("not found")});
            }
            else
            {
                std::setprecision(6);
                answers_info["curvature"] = bus_info.curvature;
                answers_info["request_id"] = id;
                answers_info["route_length"] = bus_info.route_length;
                answers_info["stop_count"] = bus_info.stops_on_route;
                answers_info["unique_stop_count"] = bus_info.unique_stops;
            }
        }
        else if (request_name == "Stop")
        {
            std::set<std::string_view> stop_info = transport_catalogue.GetStopInfo(name);
            if (*stop_info.begin() == "not found")
            {
                answers_info.insert({"request_id", id});
                answers_info.insert({"error_message", std::string("not found")});
            }
            else
            {
                if (*stop_info.begin() == "no buses")
                {
                    answers_info["buses"] = buses;
                    answers_info["request_id"] = id;
                }
                else
                {
                    answers_info["request_id"] = id;
                    for (const auto bus : stop_info)
                    {
                        buses.push_back(std::string(bus));
                    }
                    answers_info["buses"] = buses;
                }
            }
        }
        else
        {
            answers_info.insert({"request_id", id});
            std::ostringstream strm; 
            map_renderer.DrawMap(strm);
            std::string s = strm.str();
            answers_info.insert({"map", s});
        }
        return answers_info;
    }
}
