# MPM-Geomechanics simulator

## Motivation

MPM-Geomechanics is a program that allows us to model the behavior of geo-materials, like soil and rock, when these materials are subjected to different initial and boundary conditions. Currently, the geo-materials are present in several areas of the society, like for example, in the slopes and excavation process in mining industry activities, or in the study of risk associated to naturals disasters.

This program uses the Material Point Method (MPM) to integrate the motion equation of continuum mechanics. In this method the motion equation is solve in an fixed mesh (Eulerian mesh) and the solution is interpolated to points that can move freely inside the mesh, carrying all the mechanical information like strain, stress, temperature, etc. The points allows us to transport all properties inside the mesh during the simulation (Lagrangian points).

The objective of this repository is to provide a platform for developing the MPM for the study of geomechanical problems involving large deformations and distortions.

## Collaboration

If you are interested to collaborate with this project, please contact to fabricio.fernandez@ucn.cl.

## Program features

The main features of the program in the actuality are: 
- Three-dimensional formulation (can simulated 2D plane strain problems too)
- Dynamic formulation (suitable for earthquake and general dynamic problems)
- Shared memory parallelization (for computational time reduction)
- Several constitutive models for soils and rock, including softening and hardening options.

## Compilation

To compile using make:

```bash
/make$ make
```

## Documentation

To generate the code documentation using doxygen run: 

```bash
/doxygen$ doxygen
```
and the HTML documentation will be located in `/doxygen/html`.

To generate the documentation in PDF, run the make command in `doxy/latex`.

```bash
/doxygen/latex$ make
```