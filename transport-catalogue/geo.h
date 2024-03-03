#pragma once

#include <string_view>

namespace guide
{
    namespace stop_coordinate
    {
        struct StopDistances
        {
            int distance;          // расстояние до ближайшей остановки
            std::string_view stop; // название ближайшей остановки
        };

        struct Coordinates
        {
            double lat;
            double lng;
            bool operator==(const Coordinates &other) const;
            bool operator!=(const Coordinates &other) const;
        };

        double ComputeDistance(Coordinates from, Coordinates to);
    }

}