#pragma once
#include "svg.h"
#include "geo.h"
#include "domain.h"
#include "transport_catalogue.h"

#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <optional>
#include <variant>
#include <string>
#include <vector>

namespace map_renderer
{
    struct settings
    {
        double width;
        double height;
        double padding;
        double line_width;
        double stop_radius;
        int bus_label_font_size;
        svg::Point bus_label_offset;
        int stop_label_font_size;
        svg::Point stop_label_offset;
        svg::Color underlayer_color;
        double underlayer_width;
        std::vector<svg::Color> color_palette;
    };

    inline const double EPSILON = 1e-6;

    bool IsZero(double value);

    using namespace guide;
    class SphereProjector
    {
    public:
        // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                        double max_width, double max_height, double padding)
        {
            padding_ = padding;
            // Если точки поверхности сферы не заданы, вычислять нечего
            if (points_begin == points_end)
            {
                return;
            }

            // Находим точки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs)
                { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // Находим точки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs)
                { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_))
            {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // Вычисляем коэффициент масштабирования вдоль координаты y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat))
            {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom)
            {
                // Коэффициенты масштабирования по ширине и высоте ненулевые,
                // берём минимальный из них
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom)
            {
                // Коэффициент масштабирования по ширине ненулевой, используем его
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom)
            {
                // Коэффициент масштабирования по высоте ненулевой, используем его
                zoom_coeff_ = *height_zoom;
            }
        }

        // Проецирует широту и долготу в координаты внутри SVG-изображения
        svg::Point operator()(stop_coordinate::Coordinates coords) const;

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    class MapRenderer : public TransportCatalogue
    {
    public:
        void SetSettings(settings sett);

        double GetWidth();
        double GetHeight();
        double GetPadding();
        double GetLineWidth();
        double GetStopRadius();
        int GetBusLabelFontSize();
        svg::Point GetBusLabelOffset();
        int GetStopLabelFontSize();
        svg::Point GetStopLabelOffset();
        svg::Color GetUnderlayerColor();
        double GetUngerlayerWidth();
        std::vector<svg::Color> GetColorPalette();

        void DrawLines(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue);
        void DrawBusNames(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue);
        void DrawStops(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue);
        void DrawStopNames(svg::Document &doc, const SphereProjector &proj, const TransportCatalogue &transport_catalogue);
        void DrawMap(std::ostream &output, const TransportCatalogue &transport_catalogue);

    private:
        settings settings_;
    };
}