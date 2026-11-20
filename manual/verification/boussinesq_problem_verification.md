# Boussinesq Problem Verification {#boussinesq_verification}

## Problem Description
This benchmark validates the elastic stress distribution generated in **MPM-Geomechanics** against the classical analytical solution of Boussinesq for a homogeneous, isotropic, linear-elastic half-space subjected to a vertical surface load.

## Analytical Reference
For a point load $P$ acting normal to the surface of a semi-infinite elastic medium, the vertical normal stress increment $\Delta \sigma_z$ at depth $z$ and radial distance $r$ is given by:

$$\Delta \sigma_z = \frac{3 P}{2 \pi z^2} \left[ 1 + \left( \frac{r}{z} \right)^2 \right]^{-5/2}$$

## Numerical Model and Results
A 3D elastic domain is discretized with a structured background grid and sliding boundary conditions along the lateral and bottom planes to approximate semi-infinite conditions. Vertical stress profiles extracted along depth and radial cross-sections closely match the theoretical Boussinesq decay curve.