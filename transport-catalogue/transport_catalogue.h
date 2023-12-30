#pragma once
#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geo.h"

namespace guide
{
	struct BusInfo
	{
		int stops_on_route;
		int unique_stops;
		double route_length;
	};

	class TransportCatalogue
	{
	public:
		void AddStop(const std::string &name, stop_coordinate::Coordinates coordinates);

		void AddBus(const std::string &name, const std::vector<std::string_view> &stops);

		BusInfo GetBusInfo(std::string_view name) const;

		std::set<std::string_view> GetStopInfo(std::string_view name) const;

		void GetAllInfo() const;

	private:
		std::unordered_map<std::string_view, stop_coordinate::Coordinates> stops_;
		std::unordered_map<std::string_view, std::vector<std::string_view>> buses_;
		std::deque<std::string> all_items_;
		std::unordered_map<std::string_view, std::set<std::string_view>> stops_and_buses_;
	};
}
