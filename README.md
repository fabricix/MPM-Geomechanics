# MPM-Geomechanics simulator

## Motivation

MPM-Geomechanics is a program that allows us to model the behavior of geo-materials, like soil and rock, when these materials are subjected to different initial and boundary conditions. Currently, the geo-materials are present in several areas of the society, like for example, in the slopes and excavation process in mining industry activities, or in the study of risk associated to naturals disasters.

This program uses the Material Point Method (MPM) to integrate the motion equation of continuum mechanics. In this method the motion equation is solved in an fixed mesh (Eulerian mesh) and the solution is interpolated to points that can move freely inside the mesh, carrying all the mechanical information like strain, stress, temperature, etc. The points allows us to transport all properties inside the mesh during the simulation (Lagrangian points).

The objective of this repository is to provide a platform for developing the MPM for the study of geomechanical problems involving large deformations and distortions.

## Collaboration

If you are interested to collaborate with this project, please contact to fabricio.fernandez@ucn.cl. There are several topics for developing in this project, here there are a few of them:
- contact method for large terrain models using STL meshes [(Zhang et al., 2023)](https://www.sciencedirect.com/science/article/abs/pii/S0266352X23001684)
- axisymmetric formulation [(Nairn & Guilkey, 2015)](https://www.cof.orst.edu/cof/wse/faculty/Nairn/papers/AxisymGIMP.pdf)
- implicit [(Nair & Roy, 2012)](https://doi.org/10.1080/15376494.2010.550082) and semi-implicit [(Kularathna et al., 2021)](https://doi.org/10.1002/nag.3207) time integration
- convected particle domain interpolation [(Sadeghirad et al., 2011)](https://onlinelibrary.wiley.com/doi/10.1002/nme.3110)
- thermo-mechanics formulation [(Zhan et al., 2024)](https://ui.adsabs.harvard.edu/link_gateway/2024CGeot.17006313Z/doi:10.1016/j.compgeo.2024.106313)
- update stress schemes USF and MUSL [(Buzzi et al., 2008)](https://www.newcastle.edu.au/__data/assets/pdf_file/0015/22236/Caveats-on-the-implementation-of-the-generalized-material-point-method.pdf)
- damage constitutive models [(Homel & Herbold, 2016)](https://onlinelibrary.wiley.com/doi/abs/10.1002/nme.5317)
- viscous constitutive models
- critical state constitutive models [(Sheng et al., 2000)](https://www.newcastle.edu.au/__data/assets/pdf_file/0015/22560/45_Aspects-of-finite-element-implementation-of-critical-state-models.pdf)
- MPI implementation for process optimization [(Ku & Kim, 2023)](https://doi.org/10.1007/s40571-022-00480-y)
- ...

## Program features

The main features of the program in the actuality are: 
- Three-dimensional formulation (can simulated 2D plane strain problems too)
- Dynamic formulation (suitable for earthquake and general dynamic problems)
- Shared memory parallelization (for computational time reduction)
- Several constitutive models for soils and rock, including softening and hardening options.

## Compilation

To compile using make:

```bash
cd make && make
```
Make sure you have installed OpenMP.

## Documentation

To generate the code documentation using doxygen run: 

```bash
cd doxygen && doxygen
```
and the HTML documentation will be located in `/doxygen/html`.

To generate the documentation in PDF, run the make command in `doxy/latex`.

```bash
cd /doxygen/latex && make
```