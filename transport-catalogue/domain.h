#pragma once

#include <deque>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "geo.h"

namespace guide
{
    inline std::map<std::string_view, stop_coordinate::Coordinates> stops_;
    inline std::map<std::string_view, std::vector<std::string_view>> buses_;
    inline std::map<std::string_view, std::vector<std::string_view>> one_way_buses_;
    inline std::deque<std::string> all_items_;
    inline std::map<std::string_view, std::set<std::string_view>> stops_and_buses_;
    inline std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>> distances_;
    inline std::vector<stop_coordinate::Coordinates> coordinates_;
}
