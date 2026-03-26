#ifndef NASCHSTATE_HPP_
#define NASCHSTATE_HPP_

#include <iostream>
#include <nlohmann/json.hpp>
#include "road.hpp"
#include "vehicle.hpp"

//! Nasch cell state.
struct naschState {
	bool hasVehicle;
	Road roadState;
	Vehicle vehicleState;

	//! Default constructor function.
	naschState() : hasVehicle(false), roadState(), vehicleState() {}
};


std::ostream& operator<<(std::ostream& os, const naschState& x) {
	os  << "<" << ((x.hasVehicle)? 1 : 0) << "," 
	<< (x.roadState.maxSpeedLimit) << ","
	<< (x.roadState.cellInMeters) << ","
	<< (x.vehicleState.speed) << ","
	<< (x.vehicleState.vehicleType) << ">";
	return os;
}

//! The simulator must be able to compare the equality of speed, hasVehicle and road since we also consider about speed difference too
bool operator!=(const naschState& x, const naschState& y) {
	return (x.hasVehicle != y.hasVehicle) || 
	(x.roadState != y.roadState) || 
	(x.vehicleState != y.vehicleState);	}

//! It parses a JSON file and generates the corresponding naschState object.
void from_json(const nlohmann::json& j, naschState& s) {
	j.at("hasVehicle").get_to(s.hasVehicle);
	j.at("roadState").get_to(s.roadState);
	j.at("vehicleState").get_to(s.vehicleState);
	
}

#endif //NASCH_CELL_HPP_
