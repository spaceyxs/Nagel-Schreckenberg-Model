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
        double randBraking = 0.5;

         // Determine the current cell position. 
         // !! My positions are absoulte
        std::vector<int> coords;
        for (const auto& [nId, nData] : neighborhood) { coords.push_back(nId[0]); }
        std::sort(coords.begin(), coords.end());
        int myPos = coords[coords.size() / 2];

       
        // 1. Pulling logic:
        // If this cell is empty, check if a car comes into it (check behind )
        if (!state.hasVehicle) {
            int bestNeighborPos = -1; 
            
            for (const auto& [nId, nData] : neighborhood) {
                int nPos = nId[0];
                if (nPos >= myPos) continue; // If neighbor in front of me ignore
                if (!nData.state->hasVehicle) continue; // If no car in the neighbor cell ignore 
                int nSpeed = nData.state->vehicleState.speed; 
                
                //Check if there is a car between of my nPos and empty cell to avoid crashing
                int frontCarPos = -1;
                int frontCarSpeed = 0;
                
                for (const auto& [checkId, checkData] : neighborhood) {
                
                    int checkPos = checkId[0];
                    if (checkData.state->hasVehicle && checkPos > nPos) {
                        
                        if (frontCarPos ==-1 || checkPos < frontCarPos) {
                            frontCarPos = checkPos;
                            frontCarSpeed = checkData.state->vehicleState.speed;
                        }
                    }
                }
                // Check if the arriving car (nPos) needs to brake due to a car 
                if(frontCarPos != -1){
                    int frontCarTarget = frontCarPos + frontCarSpeed;
                    int myTarget = nPos + nSpeed;
                    
                    //Don't pass that car's next position
                    if(myTarget >= frontCarTarget ){
                        nSpeed = frontCarSpeed;
                    }

                    // Leave a gap while braking
                    int gap = frontCarPos - nPos - 1;
                    
                    if(nSpeed > gap){
                        nSpeed = gap;
                    }
                    if(nSpeed < 0){
                        nSpeed = 0;
                    }
                }

                // Calc distance for the incoming car
                int distance = myPos - nPos;
                
                // Does the car land exactly on this cell?
                if(nSpeed == distance && nSpeed > 0){
                    // Look nearest car
                    if (nPos > bestNeighborPos) {
                        // Update arriving car data
                        bestNeighborPos = nPos;
                        arrivingSpeed = nSpeed;
                        arrivingType = nData.state->vehicleState.vehicleType;
                        carArrives = true;
                    }
                }
            }
        } 
        
        // 2. Occupied-cell logic: If the cell has a vehicle, calculate its next move.
        // if it has enough speed to leave the current cell, move car.
        else {
            int currentV = state.vehicleState.speed;
            
            //Find closest car ahead
            int frontCarPos = -1;
            int frontCarSpeed = 0; 
            
            // Finding closest car front of my car 
            for (const auto& [nId, nData] : neighborhood) {
                int neighborPos = nId[0];
                
                if (nData.state->hasVehicle && neighborPos > myPos) {
                    if (frontCarPos ==-1 || neighborPos < frontCarPos) {
                        
                        frontCarPos = neighborPos;
                        frontCarSpeed = nData.state->vehicleState.speed;
                    }
                }
            }
            // Braking logic
            if(frontCarPos != -1){

                if(myPos +currentV >= frontCarPos + frontCarSpeed){
                    currentV = frontCarSpeed;
                }

                int gap = frontCarPos - myPos - 1;
                
                if(currentV > gap){
                    currentV = gap;
                }
                
                if(currentV < 0){
                    currentV = 0;
                }
            }

            state.vehicleState.speed = currentV;
           
            
            int myTarget = myPos + currentV;
            for (const auto& [nId, nData] : neighborhood) {
                 // Car leaves if it's target exist in neighborhood 
                if (nId[0] == myTarget && currentV > 0) {
                    carLeaves = true;
                    break;
                }
               
                carLeaves = true;  // Car leaves at the end of the road 
             
            }
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
