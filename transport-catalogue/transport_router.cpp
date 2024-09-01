#include "transport_router.h"
#include "router.h"
#include "graph.h"

#include <iostream>
#include <iterator>

namespace router
{
    TransportRouter::TransportRouter(int bus_wait_time, int bus_velocity, guide::TransportCatalogue &transport_catalogue)
        : bus_wait_time_(bus_wait_time),
          bus_velocity_(bus_velocity)
    {
        const double H_TO_M = 0.06;
        const auto stops_count = transport_catalogue.GetStopsCount();
        graph::DirectedWeightedGraph<double> graph(2 * stops_count);
        stops_names_ = transport_catalogue.GetStopsName();
        const std::map<std::string_view, std::vector<std::string_view>> &buses = transport_catalogue.GetOneWayBuses();
        for (const auto &[name, stops] : buses)
        {
            if (transport_catalogue.IsBusRound(name))
            {
                for (size_t i = 0; i < stops.size() - 1; i++)
                {
                    for (size_t j = i + 1; j < stops.size() - (i == 0 ? 1 : 0); j++)
                    {
                        // std::cout << i << " " << j << std::endl;
                        const auto stop_i = GetStopNumber(stops[i]);
                        const auto stop_j = GetStopNumber(stops[j]);
                        const auto distance_i_j = GetDistance(transport_catalogue, stops, i, j);
                        const graph::Edge<double> edge{stop_i, stop_j + stops_count, static_cast<double>(distance_i_j) / static_cast<double>(bus_velocity_) * H_TO_M};
                        bus_edges_[{stop_i, stop_j + stops_count, static_cast<double>(distance_i_j) / static_cast<double>(bus_velocity_) * H_TO_M}] = std::make_pair(name, std::abs(static_cast<int>(i) - static_cast<int>(j)));
                        graph.AddEdge(edge);
                    }
                }
                const graph::Edge<double> edge{GetStopNumber(stops[0]) + stops_count, GetStopNumber(stops[0]), static_cast<double>(bus_wait_time_)};
                graph.AddEdge(edge);
            }
            else
            {
                for (size_t i = 0; i < stops.size(); i++)
                {
                    for (size_t j = 0; j < stops.size(); j++)
                    {
                        if (i == j)
                        {
                            continue;
                        }
                        // std::cout << i << " " << j << std::endl;
                        const auto stop_i = GetStopNumber(stops[i]);
                        const auto stop_j = GetStopNumber(stops[j]);
                        const auto distance_i_j = GetDistance(transport_catalogue, stops, i, j);
                        const graph::Edge<double> edge{stop_i, stop_j + stops_count, static_cast<double>(distance_i_j) / static_cast<double>(bus_velocity_) * H_TO_M};
                        bus_edges_[{stop_i, (stop_j + stops_count), static_cast<double>(distance_i_j) / static_cast<double>(bus_velocity_) * H_TO_M}] = std::make_pair(name, std::abs(static_cast<int>(i) - static_cast<int>(j)));
                        graph.AddEdge(edge);
                    }
                }
            }
        }
        for (size_t i = 0; i < stops_names_.size(); i++)
        {
            const graph::Edge<double> edge{i + stops_count, i, static_cast<double>(bus_wait_time_)};
            graph.AddEdge(edge);
        }
        graph_ = std::move(std::make_unique<graph::DirectedWeightedGraph<double>>(graph));
        router_ = std::move(std::make_unique<graph::Router<double>>(graph::Router<double>(*graph_)));
    }

    void TransportRouter::PrintBusInfo() const
    {
        std::cout << "bus_wait_time:  " << bus_wait_time_ << std::endl;
        std::cout << "bus_velocity:  " << bus_velocity_ << std::endl;
    }


    size_t TransportRouter::GetStopNumber(std::string_view stop) const
    {
        return static_cast<size_t>(distance(stops_names_.begin(), stops_names_.find(stop)));
    }

    std::optional<std::vector<std::variant<guide::RouteWaitInfo, guide::RouteBusInfo>>> TransportRouter::GetRouteInfo(std::string_view from, std::string_view to) const
    {
        const auto route = router_->BuildRoute(GetStopNumber(from) + stops_names_.size(), GetStopNumber(to) + stops_names_.size());
        std::vector<std::variant<guide::RouteWaitInfo, guide::RouteBusInfo>> route_info;
        if (!route)
        {
            return {};
        }
        graph::Router<double>::RouteInfo info = route.value();
        for (const auto &edge : info.edges)
        {
            const auto &graph_edge = graph_->GetEdge(edge);
            if (std::abs(static_cast<int>(graph_edge.from - graph_edge.to)) == GetStopsCount())
            {
                // std::cout << "Wait" << std::endl;
                route_info.push_back(guide::RouteWaitInfo{GetStopName(graph_edge.from), GetBusTimeWait()});
            }
            else
            {
                // std::cout << "Bus" << std::endl;
                route_info.push_back(guide::RouteBusInfo{GetBus(graph_edge), GetSpanCount(graph_edge), graph_edge.weight});
            }
        }
        return route_info;
    }

    void TransportRouter::PrintGraph()
    {
        graph_->Print();
    }

    std::string TransportRouter::GetStopName(size_t edge_id) const
    {
        if (edge_id >= stops_names_.size())
        {
            edge_id -= stops_names_.size();
        }
        auto it = stops_names_.begin();
        advance(it, edge_id);
        return std::string(*it);
    }

    int TransportRouter::GetBusTimeWait() const
    {
        return bus_wait_time_;
    }

    size_t TransportRouter::GetStopsCount() const
    {
        return stops_names_.size();
    }

    int TransportRouter::GetDistance(guide::TransportCatalogue &transport_catalogue, const std::vector<std::string_view> &stops, size_t from, size_t to) const
    {
        int distance = 0;
        int step = (from > to) ? -1 : 1;

        for (size_t i = from; i != to; i += step)
        {
            distance += transport_catalogue.GetDistance(stops[i], stops[i + step]);
        }

        return distance;
    }

    std::string TransportRouter::GetBus(const graph::Edge<double> &edge) const
    {
        return bus_edges_.at({edge.from, edge.to, edge.weight}).first;
    }

    int TransportRouter::GetSpanCount(const graph::Edge<double> &edge) const
    {
        return bus_edges_.at({edge.from, edge.to, edge.weight}).second;
    }

}
