# Detailed Example – One-Way Hydro-Mechanical Coupling in Slope Stability Simulation

This document provides an explanation of a slope failure simulation incorporating one-way hydro-mechanical coupling using the MPM-Geomechanics code. The simulation is designed to evaluate the influence of a predefined pore pressure field on the mechanical behavior of a soil slope.

## Overview

In this setup, pore pressure is introduced as a fixed field (not updated during the simulation) to affect the internal force equilibrium. The material response is computed based on the effective stress, while the equilibrium equations consider total stress. This reflects a typical one-way coupling strategy.

## Files Description

- `slope-failure-coupled.json`: Input file defining geometry, materials, boundary conditions, solver settings, and coupling configuration.
- `pore-pressure-3d.json`: JSON file assigning hydrostatic pore pressures to each spatial position inside the slope.
- `generate_pore_pressure_3d_from_json.py`: Python script to build the pore pressure field based on the polygonal shape and water level.
- `config.json`: Contains the domain configuration (geometry, resolution, water level) used by the Python script in pressure generation.
- `particleTimeSerie.pvd`: Output file for ParaView to load the time series of particle data.
- `time-energy.csv`: CSV file storing time history of total kinetic energy of the system.

## Geometry and Discretization

- A 2D polygonal slope is defined in the x–z plane and extruded in the y-direction over 1 m to create a 3D body.
- Discretization length: 1.0 m in all directions.
- Domain bounds: x = [0, 110], z = [0, 35], y = [0, 1].

## Material Properties

- Model: Mohr–Coulomb plasticity with strain-softening disabled.
- Young's modulus: 100 MPa
- Poisson's ratio: 0.3
- Cohesion: 15 kPa
- Friction angle: 35 degrees
- Density: 2000 kg/m3

## Hydro-Mechanical Coupling Configuration

- Enabled via:
  ```json
  "hydro_mechanical_coupling": {
    "enabled": true,
    "type": "one_way",
    "pore_pressure_field": "pore-pressure-3d.json"
  }
  ```
- The pore pressure field is interpreted as acting in compression (positive values), and incorporated as:
  ```
  stress_total = stress_effective - p · I
  ```
## Unsaturated Stress

For unsaturated stress analysis using Bishop approach the saturations must be defined in the `pore_pressure_field` file.

Enable this feature as:
  ```json
	"hydro_mechanical_coupling": 
	{
  		"enabled":true,
  		"type": "one_way",
  		"pore_pressure_field":"pore-pressure-3d.json",
		
		"unsaturated_analysis":true,
		"bishop_chi":{
			"model":"sr",
			"sr_res":0.1,
			"constant":1.0
		}
	}
  ```
  The Chi model can be `constant` for `Chi=constant`, `sr` for `Chi=Sr`, or `se` for `Chi=Se` where `Se= (Sr-Sres)/(1-Sres)`. In witch `Sr` is the saturation, `Se` is the effective saturation, and `Sres` is the residual saturation. 

## Initial Conditions and Stability

- To ensure realistic deformation behavior, the simulation must begin from a stress-equilibrated state. This is achieved by running a preliminary gravity loading phase with:
  ```json
  	"damping":
	{
		"type":"kinetic"
	},
  "save_state": true
  ```
- Then, in the current simulation:
  ```json
  "load_state": true
  ```

## Simulation Execution

1. Generate the pore pressure field:
   ```bash
   python generate_pore_pressure_3d_from_json.py
   ```

2. Run the simulation:
   ```bash
   ./MPM-Geomechanics slope-failure-coupled.json
   ```

## Output Visualization

- Open `particleTimeSerie.pvd` in ParaView to visualize displacement, plastic strain, pressure, and stress.
- Use time navigation to observe the evolution of slope deformation over time.

## Interpretation

- The pressure field reduces the effective stress, potentially leading to earlier or deeper failure surfaces.
- A comparative run without hydro-mechanical coupling should exhibit smaller deformation, validating the coupling implementation.

## Notes

- Damping settings can be adjusted (local or kinetic) for static solution in stress field initiation.
- Ensure mesh resolution aligns with the pressure field spacing (defined in `config.json`).
- Energy conservation and convergence can be tracked via `time-energy.csv`.
