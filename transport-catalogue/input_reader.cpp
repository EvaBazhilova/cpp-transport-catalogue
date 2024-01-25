#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace guide
{

    namespace parcing_details
    {
        /**
         * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
         */
        stop_coordinate::Coordinates ParseCoordinates(std::string_view str)
        {
            static const double nan = std::nan("");

            auto not_space = str.find_first_not_of(' ');
            auto comma = str.find(',');

            if (comma == str.npos)
            {
                return {nan, nan};
            }

            auto not_space2 = str.find_first_not_of(' ', comma + 1);
            auto comma2 = str.find(',', not_space2);

            double lat;
            double lng;

            if (comma2 == str.npos)
            {
                lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
                lng = std::stod(std::string(str.substr(not_space2)));
            }
            else
            {
                lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
                lng = std::stod(std::string(str.substr(not_space2, comma2 - not_space2)));
            }

            return {lat, lng};
        }

        namespace detail
        {
            /**
             * Удаляет пробелы в начале и конце строки
             */
            std::string_view Trim(std::string_view string)
            {
                const auto start = string.find_first_not_of(' ');
                if (start == string.npos)
                {
                    return {};
                }
                return string.substr(start, string.find_last_not_of(' ') + 1 - start);
            }

            /**
             * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
             */
            std::vector<std::string_view> Split(std::string_view string, char delim)
            {
                std::vector<std::string_view> result;

                size_t pos = 0;
                while ((pos = string.find_first_not_of(' ', pos)) < string.length())
                {
                    auto delim_pos = string.find(delim, pos);
                    if (delim_pos == string.npos)
                    {
                        delim_pos = string.size();
                    }
                    if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty())
                    {
                        result.push_back(substr);
                    }
                    pos = delim_pos + 1;
                }

                return result;
            }
        }

        std::vector<stop_coordinate::StopDistances> SplitStopDistances(std::string_view info)
        {
            std::vector<stop_coordinate::StopDistances> stop_distances;
            info = detail::Trim(info);
            auto comma = info.find(' ');
            auto comma2 = info.find(' ', comma + 1);
            info = info.substr(comma2 + 1, info.size() - comma2 + 1);
            while (info.find("m to") != std::string::npos)
            {
                auto m = info.find('m');
                if (info.find(',') == std::string::npos)
                {
                    stop_distances.push_back({std::stoi(std::string(info.substr(0, m))), info.substr(m + 5, info.size() - m - 5)});
                    info = "";
                }
                else
                {
                    stop_distances.push_back({std::stoi(std::string(info.substr(0, m))), info.substr(m + 5, info.find(',') - m - 5)});
                    comma = info.find(',');
                    info = info.substr(comma+1, info.size()-comma-1);
                }
                //std::cout << stop_distances.back().distance << " " << stop_distances.back().stop << std::endl; 
            }
            return stop_distances;
        }

        /**
         * Парсит маршрут.
         * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
         * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
         */
        std::vector<std::string_view> ParseRoute(std::string_view route)
        {
            if (route.find('>') != route.npos)
            {
                return detail::Split(route, '>');
            }

            auto stops = detail::Split(route, '-');
            std::vector<std::string_view> results(stops.begin(), stops.end());
            results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

            return results;
        }

        CommandDescription ParseCommandDescription(std::string_view line)
        {
            auto colon_pos = line.find(':');
            if (colon_pos == line.npos)
            {
                return {};
            }

            auto space_pos = line.find(' ');
            if (space_pos >= colon_pos)
            {
                return {};
            }

            auto not_space = line.find_first_not_of(' ', space_pos);
            if (not_space >= colon_pos)
            {
                return {};
            }

            return {std::string(line.substr(0, space_pos)),
                    std::string(line.substr(not_space, colon_pos - not_space)),
                    std::string(line.substr(colon_pos + 1))};
        }
    }

    void FormTransportBase(std::istream &input, TransportCatalogue &transport_catalogue)
    {
        int base_request_count;
        input >> base_request_count >> std::ws;
        {
            InputReader reader;
            for (int i = 0; i < base_request_count; ++i)
            {
                std::string line;
                getline(input, line);
                reader.ParseLine(line);
            }
            reader.ApplyCommands(transport_catalogue);
        }
    }

    void InputReader::ParseLine(std::string_view line)
    {
        auto command_description = parcing_details::ParseCommandDescription(line);
        if (command_description)
        {
            commands_.push_back(std::move(command_description));
        }
    }

    void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue &catalogue) const
    {
        for (const auto &command : commands_)
        {
            if (command.command == "Stop")
            {
                catalogue.AddStop(command.id, parcing_details::ParseCoordinates(command.description));
            };
        }
        for (const auto &command : commands_)
        {
            if (command.command == "Stop" && command.description.find("m to ") != std::string::npos)
            {
                catalogue.AddDistances(command.id, parcing_details::SplitStopDistances(command.description));
            };
        }
        for (const auto &command : commands_)
        {
            if (command.command == "Bus")
            {
                catalogue.AddBus(std::move(command.id), parcing_details::ParseRoute(command.description));
            }
        }
    }
}
