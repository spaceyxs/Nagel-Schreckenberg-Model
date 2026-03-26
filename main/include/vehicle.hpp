#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <iostream>
#include <nlohmann/json.hpp>

struct Vehicle {
	int speed;
	int vehicleType;

	Vehicle() : speed(-1), vehicleType(0) {}

};


std::ostream& operator<<(std::ostream& os, const Vehicle& x) {
	os << "<" << x.speed << "," << x.vehicleType << ">";
	return os;
}

// Compare two states
bool operator!=(const Vehicle& x, const Vehicle& y) {
	return (x.speed != y.speed) ||
            (x.vehicleType != y.vehicleType);
}

// Read state from JSON file
void from_json(const nlohmann::json& j, Vehicle& s) {
	j.at("speed").get_to(s.speed);
    j.at("vehicleType").get_to(s.vehicleType);
  
}


#endif