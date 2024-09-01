#include "json_reader.h"
#include "svg.h"
#include "request_handler.h"

#include <string>
#include <vector>

namespace guide
{
    void FormTransportBase(const json::Array &base_requests, TransportCatalogue &transport_catalogue)
    {
        for (const auto &info : base_requests)
        {
            if (info.AsMap().at("type").AsString() == "Stop")
            {
                std::setprecision(6);
                transport_catalogue.AddStop(info.AsMap().at("name").AsString(), {info.AsMap().at("latitude").AsDouble(), info.AsMap().at("longitude").AsDouble()});
            }
        }
        for (const auto &info : base_requests)
        {
            if (info.AsMap().at("type").AsString() == "Stop")
            {
                std::vector<stop_coordinate::StopDistances> stop_distances = {};
                for (const auto &[stop, distances] : info.AsMap().at("road_distances").AsMap())
                {
                    stop_distances.push_back({distances.AsInt(), stop});
                }
                transport_catalogue.AddDistances(info.AsMap().at("name").AsString(), stop_distances);
            }
        }
        for (const auto &info : base_requests)
        {
            if (info.AsMap().at("type").AsString() == "Bus")
            {
                std::vector<std::string_view> stops;
                for (const auto &stop : info.AsMap().at("stops").AsArray())
                {
                    stops.push_back(stop.AsString());
                }
                if (!info.AsMap().at("is_roundtrip").AsBool())
                {
                    std::vector<std::string_view> results(stops.begin(), stops.end());
                    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());
                    transport_catalogue.AddBus(info.AsMap().at("name").AsString(), results);
                    transport_catalogue.AddOneWayBus(info.AsMap().at("name").AsString(), stops);
                }
                else
                {
                    transport_catalogue.AddBus(info.AsMap().at("name").AsString(), stops);
                    transport_catalogue.AddOneWayBus(info.AsMap().at("name").AsString(), stops);
                    transport_catalogue.AddRoundBus(info.AsMap().at("name").AsString());
                }
            }
        }
    }

    json::Array FormRequestsAnswers(const json::Array &stat_requests, guide::RequestHandler &request_handler)
    {
        json::Array answers;
        for (const auto &request : stat_requests)
        {
            if (request.AsMap().at("type").AsString() == "Bus" || request.AsMap().at("type").AsString() == "Stop")
            {
                answers.push_back(request_handler.FormBusAndStopAnswer(request.AsMap().at("id").AsInt(), request.AsMap().at("type").AsString(), request.AsMap().at("name").AsString()));
                // std::cout << "Form Bus ans Stop Answers is completed!" << std::endl;
            }
            else if (request.AsMap().at("type").AsString() == "Map")
            {
                answers.push_back(request_handler.FormMapAnswer(request.AsMap().at("id").AsInt()));
                // std::cout << "Form Map Answers is completed!" << std::endl;
            }
            else if (request.AsMap().at("type").AsString() == "Route")
            {
                answers.push_back(request_handler.FormRouteAnswer(request.AsMap().at("id").AsInt(), request.AsMap().at("from").AsString(), request.AsMap().at("to").AsString()));
                // std::cout << "Form Route Answers is completed!" << std::endl;
            }
        }
        return answers;
    }

    svg::Color ParseColor(const json::Dict &render_settings, const std::string color_type)
    {
        svg::Color color;
        if (render_settings.at(color_type).IsString())
        {
            color = svg::Color(svg::StringColor{render_settings.at(color_type).AsString()});
        }
        else if (render_settings.at(color_type).IsArray())
        {
            if (render_settings.at(color_type).AsArray().size() == 3)
            {
                color = svg::Color(svg::RGB{render_settings.at(color_type).AsArray()[0].AsInt(), render_settings.at(color_type).AsArray()[1].AsInt(), render_settings.at(color_type).AsArray()[2].AsInt()});
            }
            else
            {
                color = svg::Color(svg::RGBA{render_settings.at(color_type).AsArray()[0].AsInt(), render_settings.at(color_type).AsArray()[1].AsInt(), render_settings.at(color_type).AsArray()[2].AsInt(), render_settings.at(color_type).AsArray()[3].AsDouble()});
            }
        }
        return color;
    }

    svg::Color ParseColor(const json::Node color)
    {

        if (color.IsArray())
        {
            if (color.AsArray().size() == 3)
            {
                return svg::Color(svg::RGB{color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt()});
            }
            else
            {
                return svg::Color(svg::RGBA{color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt(), color.AsArray()[3].AsDouble()});
            }
        }
        return svg::Color(svg::StringColor{color.AsString()});
    }

    void SetRenderSettings(const json::Dict &render_settings, map_renderer::MapRenderer &map_renderer)
    {
        map_renderer::settings sett;
        sett.width = render_settings.at("width").AsDouble();
        sett.height = render_settings.at("height").AsDouble();
        sett.padding = render_settings.at("padding").AsDouble();
        sett.line_width = render_settings.at("line_width").AsDouble();
        sett.stop_radius = render_settings.at("stop_radius").AsDouble();
        sett.bus_label_font_size = render_settings.at("bus_label_font_size").AsInt();
        sett.bus_label_offset = svg::Point(render_settings.at("bus_label_offset").AsArray()[0].AsDouble(), render_settings.at("bus_label_offset").AsArray()[1].AsDouble());
        sett.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
        sett.stop_label_offset = svg::Point(render_settings.at("stop_label_offset").AsArray()[0].AsDouble(), render_settings.at("stop_label_offset").AsArray()[1].AsDouble());
        sett.underlayer_color = ParseColor(render_settings, "underlayer_color");
        sett.underlayer_width = render_settings.at("underlayer_width").AsDouble();
        std::vector<svg::Color> color_palette;
        for (auto color : render_settings.at("color_palette").AsArray())
        {
            color_palette.push_back(ParseColor(color));
        }
        sett.color_palette = std::move(color_palette);
        map_renderer.SetSettings(sett);
    }


    void FormTransportBaseAndRequests(std::istream &input, TransportCatalogue &transport_catalogue, map_renderer::MapRenderer &map_renderer, std::ostream &output)
    {
        json::Document doc = json::Load(input);
        // json::Print(doc, output);
        FormTransportBase(doc.GetRoot().AsMap().at("base_requests").AsArray(), transport_catalogue);
        // std::cerr << "Transport Base is complited!" << std::endl;
        //   transport_catalogue.GetAllInfo();
        SetRenderSettings(doc.GetRoot().AsMap().at("render_settings").AsMap(), map_renderer);
        // std::cerr << "Render Settings is complited!" << std::endl;
        router::TransportRouter transport_router(doc.GetRoot().AsMap().at("routing_settings").AsMap().at("bus_wait_time").AsInt(), doc.GetRoot().AsMap().at("routing_settings").AsMap().at("bus_velocity").AsInt(), transport_catalogue);
        // std::cerr << "Route Base is complited!" << std::endl;
        guide::RequestHandler request_handler(transport_catalogue, map_renderer, transport_router);
        json::Document requests(FormRequestsAnswers(doc.GetRoot().AsMap().at("stat_requests").AsArray(), request_handler));
        // std::cerr << "Requests Answers is complited!" << std::endl;
        json::Print(requests, output);
    }
}
