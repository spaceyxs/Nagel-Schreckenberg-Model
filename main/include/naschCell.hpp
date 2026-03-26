#ifndef NASCHCELL_HPP_
#define NASCHCELL_HPP_

#include <cmath>
#include <algorithm>
#include <random>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "naschState.hpp"

using namespace cadmium::celldevs;

//! Nasch cell.
class nasch : public GridCell<naschState, double> {
	public:
	nasch(const std::vector<int>& id, 
			const std::shared_ptr<const GridCellConfig<naschState, double>>& config
		  ): GridCell<naschState, double>(id, config) {}

	[[nodiscard]] naschState localComputation(naschState state,
const std::unordered_map<std::vector<int>, NeighborData<naschState, double>>& neighborhood) const override {
        
    bool carLeaves = false;
        bool carArrives = false;
        int arrivingSpeed = -1;
        int arrivingType = 0;
        int roadEnd = 49;

        // Finding my position
        std::vector<int> coords;
        for (const auto& [nId, nData] : neighborhood) { coords.push_back(nId[0]); }
        std::sort(coords.begin(), coords.end());
        int myPos = coords[coords.size() / 2];

        // 1. PULLING LOGIC: (Check if a car from behind moves INTO this empty cell)
        if (!state.hasVehicle) {
            int bestNeighborPos = -1; 

            for (const auto& [nId, nData] : neighborhood) {
                int nPos = nId[0];
                // Only look at neighbors behind me
                if (nData.state->hasVehicle && nPos < myPos) {
                    int nSpeed = nData.state->vehicleState.speed;
                    int maxV = nData.state->roadState.maxSpeedLimit;

                    // NaSch Acceleration logic for the neighbor
                    int expectedSpeed = std::min(nSpeed + 1, maxV);
                    
                    // Gap check for the neighbor to see if they WOULD move here
                    for (const auto& [checkId, checkData] : neighborhood) {
                        int checkPos = checkId[0];
                        if (checkData.state->hasVehicle && checkPos > nPos) {
                            // Ignore the car if it is currently at the road's exit (it leaves this turn)
                            if (checkPos < roadEnd -2) {
                                int gap = checkPos - nPos;
                                if (gap <= expectedSpeed) {
                                    expectedSpeed = std::max(0, gap - 1);
                                }
                            }
                        }
                    }

                    // If the neighbor's final calculated speed lands exactly on MY position
                    if (nPos + expectedSpeed == myPos && expectedSpeed > 0) {
                        if (nPos > bestNeighborPos) {
                            bestNeighborPos = nPos;
                            arrivingSpeed = expectedSpeed;
                            arrivingType = nData.state->vehicleState.vehicleType;
                            carArrives = true;
                        }
                    }
                }
            }
        } 
        
        // 2. OCCUPIED LOGIC: (Check if the car CURRENTLY here moves out)
        else {
            int currentV = state.vehicleState.speed;
            int maxV = state.roadState.maxSpeedLimit;

            int v_desired = std::min(currentV + 1, maxV);
            int finalV = v_desired;

            // Braking logic based on cars ahead
            for (const auto& [nId, nData] : neighborhood) {
                int neighborPos = nId[0];
                
                if (nData.state->hasVehicle && neighborPos > myPos) {
                    // Ignore the car ahead if it is currently at the road's exit
                    if (neighborPos != roadEnd) {
                        int gap = neighborPos - myPos;
                        if (gap <= v_desired) {
                            finalV = std::max(0, gap - 1);

                        }
                    }
                }
            }
           
            // MOVEMENT LOGIC
            // If speed > 0, the car is moving out of this specific cell
            if (finalV > 0) {
                carLeaves = true;
            }

            // Update current speed in state for the next step (if it stays)
            state.vehicleState.speed = finalV;
        }

        // 3. FINAL STATE TRANSITION
        // Priority: If a car leaves, the cell becomes empty first.
        if (carLeaves) {
            state.hasVehicle = false;
            state.vehicleState.speed = -1;
            state.vehicleState.vehicleType = 0;
        }
        
        // If a car was calculated to arrive here, it fills the (now potentially empty) cell.
        if (carArrives) {
            state.hasVehicle = true;
            state.vehicleState.speed = arrivingSpeed;
            state.vehicleState.vehicleType = arrivingType;
        }

        return state;
    }
    

	[[nodiscard]] double outputDelay(const naschState& state) const override {
		return 1.;
	}

};

#endif //NASCH_CELL_HPP
