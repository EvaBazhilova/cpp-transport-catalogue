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
	class TransportCatalogue
	{
	public:
		void AddStop(const std::string &name, stop_coordinate::Coordinates coordinates);

		void AddDistances(const std::string &name, const std::vector<stop_coordinate::StopDistances> &stop_distances);

		void AddBus(const std::string &name, const std::vector<std::string_view> &stops);

		void AddRoundBus(const std::string &name);

		void AddOneWayBus(const std::string &name, const std::vector<std::string_view> &stops);

		BusInfo GetBusInfo(std::string_view name) const;

		std::set<std::string_view> GetStopInfo(std::string_view name) const;

		void GetAllInfo() const;

		const std::map<std::string_view, stop_coordinate::Coordinates>& GetStops() const;

		size_t GetStopsCount() const;

		const std::map<std::string_view, std::vector<std::string_view>>& GetBuses() const;

		const std::map<std::string_view, std::vector<std::string_view>>& GetOneWayBuses() const;

		const std::map<std::string_view, std::set<std::string_view>>& GetStopAndBuses() const;

		const std::vector<stop_coordinate::Coordinates>& GetCoordinates() const;

		std::set<std::string_view> GetStopsName() const;

		int GetDistance(std::string_view from, std::string_view to) const;

		bool IsBusRound (std::string_view bus) const;

	private:
		std::map<std::string_view, stop_coordinate::Coordinates> stops_;
		std::map<std::string_view, std::vector<std::string_view>> buses_;
		std::map<std::string_view, std::vector<std::string_view>> one_way_buses_;
		std::set<std::string_view> round_buses_;
		std::deque<std::string> all_items_;
		std::map<std::string_view, std::set<std::string_view>> stops_and_buses_;
		std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>> distances_;
		std::vector<stop_coordinate::Coordinates> coordinates_;
	};
}
