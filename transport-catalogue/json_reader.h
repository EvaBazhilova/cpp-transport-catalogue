#pragma once

#include <string>
#include <vector>

#include "transport_catalogue.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace guide
{

    struct CommandDescription
    {
        // Определяет, задана ли команда (поле command непустое)
        explicit operator bool() const
        {
            return !command.empty();
        }

        bool operator!() const
        {
            return !operator bool();
        }

        std::string command;     // Название команды
        std::string id;          // id маршрута или остановки
        std::string description; // Параметры команды
    };

    void FormTransportBase(const json::Array &base_requests, TransportCatalogue &transport_catalogue);
    json::Array FormRequestsAnswers(const json::Array &stat_requests, guide::RequestHandler &request_handler);
    void FormTransportBaseAndRequests(std::istream &input, TransportCatalogue &transport_catalogue, map_renderer::MapRenderer &map_renderer, std::ostream &output);

}
