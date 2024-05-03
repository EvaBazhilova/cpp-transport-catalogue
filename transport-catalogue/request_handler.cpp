#include "request_handler.h"
#include "transport_catalogue.h"
#include "json_builder.h"

#include <iostream>
#include <sstream>

namespace guide
{
    json::Node ParseStat(const TransportCatalogue &transport_catalogue, map_renderer::MapRenderer &map_renderer, int id, std::string request_name, std::string name)
    {
        json::Builder answers_info;
        answers_info.StartDict();
        if (request_name == "Bus")
        {
            //std::cout << "Bus" << std::endl;
            BusInfo bus_info = transport_catalogue.GetBusInfo(name);

            if (bus_info.stops_on_route == 0)
            {
                answers_info.Key("request_id").Value(id).Key("error_message").Value(std::string("not found"));
            }
            else
            {
                std::setprecision(6);
                //std::cout << bus_info.curvature << std::endl;
                answers_info.Key("curvature").Value(bus_info.curvature).Key("request_id").Value(id);
                answers_info.Key("route_length").Value(bus_info.route_length).Key("stop_count").Value(bus_info.stops_on_route);
                answers_info.Key("unique_stop_count").Value(bus_info.unique_stops);
            }
        }
        else if (request_name == "Stop")
        {
            //std::cout << "Stop" << std::endl;
            std::set<std::string_view> stop_info = transport_catalogue.GetStopInfo(name);
            if (*stop_info.begin() == "not found")
            {
                answers_info.Key("request_id").Value(id).Key("error_message").Value(std::string("not found"));
            }
            else
            {
                if (*stop_info.begin() == "no buses")
                {
                    answers_info.Key("buses").StartArray().EndArray().Key("request_id").Value(id);
                }
                else
                {
                    answers_info.Key("request_id").Value(id).Key("buses").StartArray();
                    for (const auto bus : stop_info)
                    {
                        answers_info.Value(std::string(bus));
                    }
                    answers_info.EndArray();
                }
            }
        }
        else
        {
            //std::cout << "Map" << std::endl;
            answers_info.Key("request_id"s).Value(id);
            std::ostringstream strm; 
            map_renderer.DrawMap(strm, transport_catalogue);
            std::string s = strm.str();
            answers_info.Key("map"s).Value(s);
        }
        answers_info.EndDict();
        return answers_info.Build();
    }
}
