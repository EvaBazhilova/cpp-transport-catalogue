#pragma once
#include <algorithm>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "domain.h"

namespace guide
{
	struct BusInfo
	{
		int stops_on_route;
		int unique_stops;
		double route_length;
		double curvature;
	};

	class TransportCatalogue
	{
	public:
		void AddStop(const std::string &name, stop_coordinate::Coordinates coordinates);

		void AddDistances(const std::string &name, const std::vector<stop_coordinate::StopDistances> &stop_distances);

		void AddBus(const std::string &name, const std::vector<std::string_view> &stops);

		void AddOneWayBus(const std::string &name, const std::vector<std::string_view> &stops);

		BusInfo GetBusInfo(std::string_view name) const;

		std::set<std::string_view> GetStopInfo(std::string_view name) const;

		void GetAllInfo() const;

	};
}
