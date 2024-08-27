#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include "graph.h"

#include <optional>
#include <memory>

namespace router
{
    class TransportRouter
    {
    public:
        TransportRouter &SetBusWaitTime(int bus_wait_time);

        TransportRouter &SetBusVelocity(int bus_velocity);

        void FormTransportRouter(guide::TransportCatalogue &transport_catalogue);

        void GetBusInfo() const;

        const std::unique_ptr<graph::DirectedWeightedGraph<double>>& GetGraph() const;

        std::optional<graph::Router<double>::RouteInfo> BuildRoute(std::string_view from, std::string_view to) const;

        void PrintGraph();

        std::string GetStopName(size_t edge_id) const;

        int GetBusTimeWait() const;

        size_t GetStopsCount() const;

        std::string GetBus(const graph::Edge<double> &edge) const;

        int GetSpanCount(const graph::Edge<double> &edge) const;

    private:
        int bus_wait_time_ = 0;
        int bus_velocity_ = 0;
        std::set<std::string_view> stops_names_;
        std::map<std::tuple<size_t, size_t, double>, std::pair<std::string, int>> bus_edges_;
        std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
        std::unique_ptr<graph::Router<double>> router_;

        size_t GetStopNumber(std::string_view stop) const;

        int GetDistance(guide::TransportCatalogue &transport_catalogue, const std::vector<std::string_view> &stops, size_t from, size_t to) const;
    };
}
