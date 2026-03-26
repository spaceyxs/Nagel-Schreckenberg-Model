# Nagel-Schreckenberg Model

This project implements the Nagel-Schreckenberg traffic model.

## Overview

This project models and simulates one-lane traffic flow using the **Nagel–Schreckenberg (NaSch) model** within the **Cell-DEVS** formalism of the **Cadmium** simulation framework.

Traffic congestion is an important issue in transportation systems, especially on roads where many vehicles move in the same direction and must respond to the available space ahead. This project focuses on representing how vehicles accelerate, slow down, and interact with each other in a one-dimensional traffic system.

The road is modeled as a one-dimensional cell space, where each cell represents a position on the road. Each cell may either be empty or occupied by a vehicle. Vehicles follow simple update rules based on their speed and the distance to the vehicle ahead.

## Objective

The objective of this project is to simulate one-lane traffic behavior and observe how traffic density affects vehicle movement under two different conditions:

- **Non-congested traffic**
- **Congested traffic**

## Model Description

The Nagel–Schreckenberg model is a cellular automata traffic model in which:

- The road is divided into discrete cells
- Each cell can contain at most one vehicle
- Each vehicle has a discrete speed
- Vehicle behavior is updated step by step using neighborhood.

At each simulation step, a vehicle follows these rules:

1. **Acceleration:** A vehicle increases its velocity by 1 unit, up to the maximum allowed velocity.
2. **Deceleration:** If the distance to the next vehicle ahead is smaller than the current velocity, the vehicle slows down to avoid a collision.
3. **Movement:** Each vehicle moves forward according to its updated velocity.

Although the rules are simple, the model can still capture important traffic behaviors such as:

- Traffic density effects
- Smooth traffic flow
- Congestion formation

## Requirements

- Cadmium / C++
- nlohmann/json

## How to Run

To build the project, run:

```bash
source build_sim.sh
```
To run example many:
```bash
./bin/nasch_model config/example_many/naschVisualization_config.json
```
To run example few:
```bash
./bin/nasch_model config/example_few/naschVisualization_config.json
```


## Visualization Notes
Open https://devssim.carleton.ca/cell-devs-viewer/

To observe the simulation step by step, place the `nasch_log.csv` file and the `naschVisualization_config.json` file of the scenario you want to view from the `config` folder into the visualization tool.

To see the vehicle movement more smoothly, instead of pressing the **Play** button, drag the slider bar manually.

This makes the vehicle flow appear more realistic and allows you to better observe how the cars move through the road.