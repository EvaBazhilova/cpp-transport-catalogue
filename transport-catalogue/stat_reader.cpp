#include "stat_reader.h"
#include "transport_catalogue.h"

namespace guide
{

    void GetRequests(const TransportCatalogue &transport_catalogue, std::istream &input, std::ostream &output)
    {
        int stat_request_count;
        input >> stat_request_count >> std::ws;
        for (int i = 0; i < stat_request_count; ++i)
        {
            std::string line;
            getline(input, line);
            ParseAndPrintStat(transport_catalogue, line, output);
        }
    };
    void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                           std::ostream &output)
    {
        std::string_view request_name = request.substr(0, request.find(" "));
        if (request_name == "Bus")
        {
            std::string_view number = request.substr(request.find(" ") + 1);

            BusInfo bus_info = transport_catalogue.GetBusInfo(number);

            if (bus_info.stops_on_route == 0)
            {
                output << request << ": not found" << std::endl;
            }
            else
            {
                std::setprecision(6);
                output << request << ": " << bus_info.stops_on_route << " stops on route, " << bus_info.unique_stops << " unique stops, " << bus_info.route_length << " route length" << std::endl;
            }
        }
        else if (request_name == "Stop")
        {
            std::string_view stop_name = request.substr(request.find(" ") + 1);
            std::set<std::string_view> stop_info = transport_catalogue.GetStopInfo(stop_name);
            if (*stop_info.begin() == "not found" || *stop_info.begin() == "no buses")
            {
                output << request << ": " << *stop_info.begin() << std::endl;
            }
            else
            {
                output << request << ": buses ";
                for (const auto bus : stop_info)
                {
                    output << bus << " ";
                }
                output << std::endl;
            }
        }
    }
}
