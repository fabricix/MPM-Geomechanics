# Base Acceleration Verification {#base_acceleration_verification}

## Problem Description
This verification test evaluates the implementation of dynamic seismic boundary conditions in **MPM-Geomechanics** by subjecting a 1D/3D elastic soil column to a prescribed base acceleration time history.

## Governing Equations
Under a basal horizontal acceleration $\ddot{u}_g(t)$, the dynamic response of an elastic layer of height $H$, shear modulus $G$, and density $\rho$ is governed by the 1D shear wave propagation equation:

$$\rho \frac{\partial^2 u}{\partial t^2} = G \frac{\partial^2 u}{\partial z^2} - \rho \ddot{u}_g(t)$$

with shear wave velocity $v_s = \sqrt{G / \rho}$.

## Numerical Model and Results
* **Boundary Conditions:** Lateral boundaries are constrained to allow pure shear deformation (periodic/sliding conditions), while the base nodes receive the prescribed acceleration input.
* **Results:** Relative displacement and acceleration time histories recorded at the top surface of the column match the analytical wave propagation and natural frequency response.