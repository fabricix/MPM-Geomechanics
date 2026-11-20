## Introduction to the Material Point Method (MPM) {#mpm_intro}

The Material Point Method (MPM) is a particle-based, meshfree numerical method well-suited for geomechanics problems involving large deformations, history-dependent materials, and multi-body contact.

In MPM, a continuum body is discretized into a set of Lagrangian material points (particles) that carry all physical properties—such as mass, volume, position, velocity, stress, strain, and internal state variables. Simultaneously, a background Eulerian computational grid is used to solve the governing equations of motion. At the end of each time step, the deformed grid is discarded and reset, eliminating the mesh entanglement and distortion issues typical of the Finite Element Method (FEM) under large deformations.

@image html mpm_cycle.png "MPM computational cycle" width=350px

A computational MPM cycle consists in a) interpolate values from particles to nodes, b) solve equations in nodes, d) interpolate solution from nodes to particles, and c) update particle's position."