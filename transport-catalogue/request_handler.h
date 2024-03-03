#pragma once

#include <iomanip>
#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"

namespace guide
{
    json::Node ParseStat(const TransportCatalogue &transport_catalogue, map_renderer::MapRenderer &map_renderer, int id, std::string request_name, std::string name);
}
