# Undrained Strength Ratio Verification {#strength_stress_vertical}

## Problem Description
This example verifies the initialization of a depth-dependent undrained shear strength profile proportional to the vertical effective stress ($S_u / \sigma_v = 0.001$) in a 2D slope model.

## Two-Stage Modeling Workflow
1. **Geostatic Elastic Equilibrium:** An initial simulation is performed using linear-elastic parameters and kinetic/local damping until steady-state equilibrium is achieved. The resulting stress field is exported via `"save_state": true`.
2. **Strength Profile Assignment:** The equilibrated stress state is imported into the second stage using `"load_state": true`, and the undrained shear strength ratio is assigned via `"su_vertical_stress": 0.001`.

## Verification Results
The computed particle state variables confirm that the undrained shear strength $S_u$ scales linearly with the initial vertical stress $\sigma_{zz}$ across the slope geometry.

@image html initial-elastic-stress-field.png "Initial elastic stress field."
@image html su-factor-initial-step.png "Su factor obtained from initial stress field."
@image html SUu-SigmaZZ-vertical-box.png "Stress Su relation along the vertical plane defined by colored particles."