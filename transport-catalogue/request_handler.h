#pragma once

#include <iomanip>
#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
#include "json_builder.h"
#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace guide
{
    class RequestHandler
    {
    public:
        RequestHandler(const TransportCatalogue &transport_catalogue, map_renderer::MapRenderer &map_renderer, const router::TransportRouter &transport_router)
            : transport_catalogue_(transport_catalogue),
              map_renderer_(map_renderer),
              transport_router_(transport_router)
        {
        }
        json::Node FormBusAndStopAnswer(int id, std::string request_name, std::string name);
        json::Node FormMapAnswer(int id);
        json::Node FormRouteAnswer(int id, std::string stop_from, std::string stop_to);

    private:
        const TransportCatalogue &transport_catalogue_;
        map_renderer::MapRenderer &map_renderer_;
        const router::TransportRouter &transport_router_;
    };
}
