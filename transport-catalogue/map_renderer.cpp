#include "map_renderer.h"

namespace map_renderer
{
    bool IsZero(double value)
    {
        return std::abs(value) < EPSILON;
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point SphereProjector::operator()(stop_coordinate::Coordinates coords) const
    {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
    }

    void MapRenderer::SetSettings(settings sett)
    {
        settings_ = std::move(sett);
    }

    double MapRenderer::GetWidth()
    {
        return settings_.width;
    }
    double MapRenderer::GetHeight()
    {
        return settings_.height;
    }
    double MapRenderer::GetPadding()
    {
        return settings_.padding;
    }
    double MapRenderer::GetLineWidth()
    {
        return settings_.line_width;
    }
    double MapRenderer::GetStopRadius()
    {
        return settings_.stop_radius;
    }
    int MapRenderer::GetBusLabelFontSize()
    {
        return settings_.bus_label_font_size;
    }
    svg::Point MapRenderer::GetBusLabelOffset()
    {
        return settings_.bus_label_offset;
    }
    int MapRenderer::GetStopLabelFontSize()
    {
        return settings_.stop_label_font_size;
    }
    svg::Point MapRenderer::GetStopLabelOffset()
    {
        return settings_.stop_label_offset;
    }
    svg::Color MapRenderer::GetUnderlayerColor()
    {
        return settings_.underlayer_color;
    }
    double MapRenderer::GetUngerlayerWidth()
    {
        return settings_.underlayer_width;
    }
    std::vector<svg::Color> MapRenderer::GetColorPalette()
    {
        return settings_.color_palette;
    }

    void MapRenderer::DrawLines(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue)
    {
        auto buses = transport_catalogue.GetBuses();
        auto bus_stops = transport_catalogue.GetStops();
        size_t count = 0;
        const size_t bus_count = buses.size() - 1;
        for (const auto &[bus, stops] : buses)
        {
            if (!stops.empty())
            {
                if (count > bus_count || count > (settings_.color_palette.size() - 1))
                {
                    count = 0;
                }
                svg::Polyline polyline;
                for (const auto &stop : stops)
                {
                    polyline.AddPoint(proj(bus_stops.at(stop)));
                    polyline.SetStrokeColor(settings_.color_palette[count]);
                }
                polyline.SetFillColor(svg::StringColor("none"));
                polyline.SetStrokeWidth(settings_.line_width);
                polyline.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                polyline.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                doc.Add(polyline);
                count++;
            }
        }
    }
    void MapRenderer::DrawBusNames(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue)
    {
        auto buses = transport_catalogue.GetBuses();
        auto one_way_buses = transport_catalogue.GetOneWayBuses();
        auto bus_stops = transport_catalogue.GetStops();
        size_t count = 0;
        const size_t bus_count = buses.size() - 1;
        for (const auto &[bus, stops] : one_way_buses)
        {
            if (!stops.empty())
            {
                if (count > bus_count || count > (settings_.color_palette.size() - 1))
                {
                    count = 0;
                }
                svg::Text text1;
                svg::Text text2;
                text1.SetFillColor(settings_.underlayer_color);
                text1.SetStrokeColor(settings_.underlayer_color);
                text1.SetStrokeWidth(settings_.underlayer_width);
                text1.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                text1.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                text1.SetPosition(proj(bus_stops.at(stops[0])));
                text1.SetOffset(settings_.bus_label_offset);
                text1.SetFontSize(static_cast<uint32_t>(settings_.bus_label_font_size));
                text1.SetFontFamily("Verdana");
                text1.SetFontWeight("bold");
                text1.SetData(std::string(bus));
                text2.SetFillColor(settings_.color_palette[count]);
                text2.SetPosition(proj(bus_stops.at(stops[0])));
                text2.SetOffset(settings_.bus_label_offset);
                text2.SetFontSize(static_cast<uint32_t>(settings_.bus_label_font_size));
                text2.SetFontFamily("Verdana");
                text2.SetFontWeight("bold");
                text2.SetData(std::string(bus));
                doc.Add(text1);
                doc.Add(text2);
                if (stops[0] != stops[stops.size() - 1])
                {
                    svg::Text adding_text1;
                    svg::Text adding_text2;
                    adding_text1.SetFillColor(settings_.underlayer_color);
                    adding_text1.SetStrokeColor(settings_.underlayer_color);
                    adding_text1.SetStrokeWidth(settings_.underlayer_width);
                    adding_text1.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                    adding_text1.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                    adding_text1.SetPosition(proj(bus_stops.at(stops[stops.size() - 1])));
                    adding_text1.SetOffset(settings_.bus_label_offset);
                    adding_text1.SetFontSize(static_cast<uint32_t>(settings_.bus_label_font_size));
                    adding_text1.SetFontFamily("Verdana");
                    adding_text1.SetFontWeight("bold");
                    adding_text1.SetData(std::string(bus));
                    adding_text2.SetFillColor(settings_.color_palette[count]);
                    adding_text2.SetPosition(proj(bus_stops.at(stops[stops.size() - 1])));
                    adding_text2.SetOffset(settings_.bus_label_offset);
                    adding_text2.SetFontSize(static_cast<uint32_t>(settings_.bus_label_font_size));
                    adding_text2.SetFontFamily("Verdana");
                    adding_text2.SetFontWeight("bold");
                    adding_text2.SetData(std::string(bus));
                    doc.Add(adding_text1);
                    doc.Add(adding_text2);
                }
                count++;
            }
        }
    }
    void MapRenderer::DrawStops(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue)
    {
        auto stops_and_buses = transport_catalogue.GetStopAndBuses();
        auto stops = transport_catalogue.GetStops();
        for (const auto &[stop, buses] : stops_and_buses)
        {
            if (!buses.empty())
            {
                svg::Circle circle;
                circle.SetCenter(proj(stops.at(stop)));
                circle.SetRadius(settings_.stop_radius);
                circle.SetFillColor(svg::StringColor("white"));
                doc.Add(circle);
            }
        }
    }
    void MapRenderer::DrawStopNames(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue)
    {
        auto stops_and_buses = transport_catalogue.GetStopAndBuses();
        auto stops = transport_catalogue.GetStops();
        for (const auto &[stop, buses] : stops_and_buses)
        {
            if (!buses.empty())
            {
                svg::Text text1;
                svg::Text text2;
                text1.SetFillColor(settings_.underlayer_color);
                text1.SetStrokeColor(settings_.underlayer_color);
                text1.SetStrokeWidth(settings_.underlayer_width);
                text1.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
                text1.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                text1.SetPosition(proj(stops.at(stop)));
                text1.SetOffset(settings_.stop_label_offset);
                text1.SetFontSize(static_cast<uint32_t>(settings_.stop_label_font_size));
                text1.SetFontFamily("Verdana");
                text1.SetData(std::string(stop));
                text2.SetFillColor(svg::StringColor("black"));
                text2.SetPosition(proj(stops.at(stop)));
                text2.SetOffset(settings_.stop_label_offset);
                text2.SetFontSize(static_cast<uint32_t>(settings_.stop_label_font_size));
                text2.SetFontFamily("Verdana");
                text2.SetData(std::string(stop));
                doc.Add(text1);
                doc.Add(text2);
            }
        }
    }
    void MapRenderer::DrawMap(std::ostream &output, const TransportCatalogue &transport_catalogue)
    {
        auto coordinates = transport_catalogue.GetCoordinates();
        auto end = coordinates.end();
        for (auto it = coordinates.begin(); it != end; ++it)
        {
            end = std::remove(it + 1, end, *it);
        }

        coordinates.erase(end, coordinates.end());
        // Создаём проектор сферических координат на карту
        const SphereProjector proj{coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding};

        svg::Document doc;

        DrawLines(doc, proj, transport_catalogue);
        DrawBusNames(doc, proj, transport_catalogue);
        DrawStops(doc, proj, transport_catalogue);
        DrawStopNames(doc, proj, transport_catalogue);
        doc.Render(output);
    }
}