#pragma once

#include <iosfwd>
#include <iostream>
#include <iomanip>
#include <string_view>

#include "transport_catalogue.h"

namespace guide
{
    void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                           std::ostream &output);
}
