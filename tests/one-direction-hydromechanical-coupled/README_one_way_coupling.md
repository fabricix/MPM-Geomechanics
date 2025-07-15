# One-Way Hydro-Mechanical Coupling Example – Slope Failure Simulation

This example demonstrates a slope stability analysis using one-way hydro-mechanical coupling in the MPM-Geomechanics simulator. The model considers pore pressure as a known field that influences the mechanical behavior but is not updated during the simulation.

## Files

- `slope-failure-coupled.json`: main configuration file for the simulation.
- `pore-pressure-3d.json`: input file defining pore pressure at 3D spatial locations.
- `generate_pore_pressure_3d_from_json.py`: script used to generate `pore-pressure-3d.json`.
- `config.json`: defines geometry, water level, and discretization for the pressure field.
- `time-energy.csv`: optional output to monitor energy evolution.

## Model Setup

- Geometry: 2D polygonal slope extruded in the Y direction (1 m thick).
- Discretization: 1 m in all directions.
- Material: Mohr–Coulomb (cohesion = 15 kPa, friction angle = 35°, E = 100 MPa).
- Gravity: Applied in the Z direction.
- Time: 5 seconds total, using critical time step × 0.25.

## Hydro-Mechanical Coupling

- Type: One-way
- Field: `pore-pressure-3d.json`, generated based on a water level at z = 18 m.
- Usage: Pressure is applied as a body force in the total stress used for equilibrium.
- The constitutive model continues to use effective stress.

## How to Run

1. (Optional) Generate the pore pressure field:
   ```
   python generate_pore_pressure_3d_from_json.py
   ```

2. Run the simulation:
   ```
   ./MPM-Geomechanics slope-failure-coupled.json
   ```

## Results and Visualization

- Results are stored in VTU files per time step.
- Open `particleTimeSerie.pvd` in ParaView to visualize displacements, pressure, stress, and plastic zones.

## Notes

- Initial stress conditions must be loaded (`"load_state": true`) to avoid numerical instability.
- The one-way coupling uses `σ_total = σ_eff - p · I` in force calculations.
