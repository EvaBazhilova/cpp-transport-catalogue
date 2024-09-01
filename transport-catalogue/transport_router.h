#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include "graph.h"
#include "domain.h"

#include <optional>
#include <memory>
#include <variant>
#include <vector>
#include <string>

namespace router
{
    class TransportRouter
    {
    public:
        TransportRouter(int bus_wait_time, int bus_velocity, guide::TransportCatalogue &transport_catalogue);

        std::optional<std::vector<std::variant<guide::RouteWaitInfo, guide::RouteBusInfo>>> GetRouteInfo(std::string_view from, std::string_view to) const;

        void PrintGraph();

        void PrintBusInfo() const;

    private:
        int bus_wait_time_ = 0;
        int bus_velocity_ = 0;
        std::set<std::string_view> stops_names_;
        std::map<std::tuple<size_t, size_t, double>, std::pair<std::string, int>> bus_edges_;
        std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
        std::unique_ptr<graph::Router<double>> router_;

        size_t GetStopNumber(std::string_view stop) const;

        int GetDistance(guide::TransportCatalogue &transport_catalogue, const std::vector<std::string_view> &stops, size_t from, size_t to) const;

        std::string GetStopName(size_t edge_id) const;

        int GetBusTimeWait() const;

        size_t GetStopsCount() const;

        std::string GetBus(const graph::Edge<double> &edge) const;

        int GetSpanCount(const graph::Edge<double> &edge) const;
    };
}
