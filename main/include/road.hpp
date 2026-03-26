#ifndef ROAD_HPP
#define ROAD_HPP

#include <iostream>
#include <nlohmann/json.hpp>

struct Road {
	int maxSpeedLimit;
	double cellInMeters;
  
	Road() : maxSpeedLimit(-1),cellInMeters(7.5){}
};


std::ostream& operator<<(std::ostream& os, const Road& x) {
	os << "<" << x.maxSpeedLimit << "," << x.cellInMeters << ">";
	return os;
}

// Compare two states
bool operator!=(const Road& x, const Road& y) {
	return (x.maxSpeedLimit != y.maxSpeedLimit) || (x.cellInMeters != y.cellInMeters);
}
// Read state from JSON file
void from_json(const nlohmann::json& j, Road& s) {
	j.at("maxSpeedLimit").get_to(s.maxSpeedLimit);
	j.at("cellInMeters").get_to(s.cellInMeters);
}

#endif