#pragma once

#include <deque>
#include <iomanip>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "geo.h"

namespace guide
{
    struct BusInfo
    {
        int stops_on_route;
        int unique_stops;
        double route_length;
        double curvature;
    };

    struct RouteWaitInfo
    {
        std::string stop_name;
        int time;
    };

    struct RouteBusInfo
    {
        std::string bus;
        int span_count;
        double time;
    };
}
