#include "request_handler.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include "json_builder.h"
#include "router.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace guide
{
    json::Node RequestHandler::FormBusAndStopAnswer(int id, std::string request_name, std::string name)
    {
        json::Builder answers_info;
        answers_info.StartDict();
        if (request_name == "Bus")
        {
            // std::cout << "Bus" << std::endl;
            BusInfo bus_info = transport_catalogue_.GetBusInfo(name);

            if (bus_info.stops_on_route == 0)
            {
                answers_info.Key("request_id").Value(id).Key("error_message").Value(std::string("not found"));
            }
            else
            {
                std::setprecision(6);
                // std::cout << bus_info.curvature << std::endl;
                answers_info.Key("curvature").Value(bus_info.curvature).Key("request_id").Value(id);
                answers_info.Key("route_length").Value(bus_info.route_length).Key("stop_count").Value(bus_info.stops_on_route);
                answers_info.Key("unique_stop_count").Value(bus_info.unique_stops);
            }
        }
        else
        {
            // std::cout << "Stop" << std::endl;
            std::set<std::string_view> stop_info = transport_catalogue_.GetStopInfo(name);
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
        answers_info.EndDict();
        return answers_info.Build();
    }

    json::Node RequestHandler::FormMapAnswer(int id)
    {
        json::Builder answers_info;
        answers_info.StartDict();

        // std::cout << "Map" << std::endl;
        answers_info.Key("request_id"s).Value(id);
        std::ostringstream strm;
        map_renderer_.DrawMap(strm, transport_catalogue_);
        std::string s = strm.str();
        answers_info.Key("map"s).Value(s);

        answers_info.EndDict();
        return answers_info.Build();
    }

    json::Node RequestHandler::FormRouteAnswer(int id, std::string stop_from, std::string stop_to)
    {
        // std::cout << "Route" << std::endl;

        json::Builder answers_info;
        answers_info.StartDict();
        if (stop_from == stop_to)
        {
            answers_info.Key("items");
            answers_info.StartArray().EndArray();
            answers_info.Key("request_id").Value(id).Key("total_time").Value(0);
        }
        else
        {
            std::optional<typename graph::Router<double>::RouteInfo> route = transport_router_.BuildRoute(stop_from, stop_to);
            if (!route)
            {
                answers_info.Key("request_id").Value(id).Key("error_message").Value(std::string("not found"));
            }
            else
            {
                graph::Router<double>::RouteInfo info = route.value();
                answers_info.Key("items");
                answers_info.StartArray();
                std::setprecision(6);
                double total_time = 0.0;
                const auto &graph = transport_router_.GetGraph();
                for (const auto &edge : info.edges)
                {
                    const auto& graph_edge = graph->GetEdge(edge);
                    // graph.GetEdge(edge).Print();
                    if (std::abs(static_cast<int>(graph_edge.from - graph_edge.to)) == transport_router_.GetStopsCount())
                    {
                        // std::cout << "Wait" << std::endl;
                        answers_info.StartDict().Key("type").Value("Wait").Key("stop_name").Value(transport_router_.GetStopName(graph_edge.from)).Key("time").Value(transport_router_.GetBusTimeWait()).EndDict();
                        total_time += graph_edge.weight;
                    }
                    else
                    {
                        // std::cout << "Bus" << std::endl;
                        answers_info.StartDict().Key("type").Value("Bus").Key("bus").Value(transport_router_.GetBus(graph_edge)).Key("span_count").Value(transport_router_.GetSpanCount(graph_edge)).Key("time").Value(graph_edge.weight).EndDict();
                        total_time += graph_edge.weight;
                    }
                }
                std::setprecision(6);
                answers_info.EndArray().Key("request_id").Value(id).Key("total_time").Value(total_time);
            }
        }

        answers_info.EndDict();
        return answers_info.Build();
    }
}
