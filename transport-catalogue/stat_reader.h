#pragma once 
 
#include <iomanip> 
#include <iosfwd> 
#include <iostream> 
#include <string_view> 
 
#include "transport_catalogue.h" 
 
namespace guide 
{ 
    void GetRequests (const TransportCatalogue &transport_catalogue, std::istream &input, std::ostream &output); 
 
    void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request, 
                           std::ostream &output); 
} 
 