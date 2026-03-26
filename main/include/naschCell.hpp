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

         // Determine the current cell position. 
         // !! My positions are absoulte :/
        std::vector<int> coords;
        for (const auto& [nId, nData] : neighborhood) { coords.push_back(nId[0]); }
        std::sort(coords.begin(), coords.end());
        int myPos = coords[coords.size() / 2];

        // 1. Pulling logic:
        // If this cell is empty, check if a car comes here
        if (!state.hasVehicle) {
            int bestNeighborPos = -1; 

            for (const auto& [nId, nData] : neighborhood) {
                int nPos = nId[0];

                // Check cars behind this cell
                if (nData.state->hasVehicle && nPos < myPos) {
                    int nSpeed = nData.state->vehicleState.speed;
                    int maxV = nData.state->roadState.maxSpeedLimit;

                    // // Increase speed (NaSch acceleration rule)
                    int expectedSpeed = std::min(nSpeed + 1, maxV);
                    
                    // Check cars in front
                    for (const auto& [checkId, checkData] : neighborhood) {
                        int checkPos = checkId[0];
                        if (checkData.state->hasVehicle && checkPos > nPos) {
                            
                            
                            // Ignore the car at the end of the road
                            if (checkPos < roadEnd -2) {
                                int gap = checkPos - nPos;
                                if (gap <= expectedSpeed) {
                                    expectedSpeed = std::max(0, gap - 1);
                                }
                            }
                        }
                    }

                    // If the car moves to this cell
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
        
        // 2. Occupied-cell logic:
        // If this cell has a car, check if it leaves
        else {
            int currentV = state.vehicleState.speed;
            int maxV = state.roadState.maxSpeedLimit;

            int v_desired = std::min(currentV + 1, maxV);
            int finalV = v_desired;

            // Braking logic based on cars in front
            for (const auto& [nId, nData] : neighborhood) {
                int neighborPos = nId[0];
                
                if (nData.state->hasVehicle && neighborPos > myPos) {
                    // Ignore the car at the road end
                    if (neighborPos != roadEnd) {
                        int gap = neighborPos - myPos;
                        if (gap <= v_desired) {
                            finalV = std::max(0, gap - 1);

                        }
                    }
                }
            }
           
            //3. Movement Logic:
            // Car leaves this cell
            if (finalV > 0) {
                carLeaves = true;
            }

            state.vehicleState.speed = finalV;
        }

        // Make cell empty if car leaves
        if (carLeaves) {
            state.hasVehicle = false;
            state.vehicleState.speed = -1;
            state.vehicleState.vehicleType = 0;
        }
        
         // Put new car in cell if one arrives
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
