# MPM-Geomechanics
### A material point method code for geomechanics

[![CI](https://github.com/fabricix/MPM-Geomechanics/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/fabricix/MPM-Geomechanics/actions/workflows/c-cpp.yml) 
[![MSBuild](https://github.com/fabricix/MPM-Geomechanics/actions/workflows/msbuild.yml/badge.svg)](https://github.com/fabricix/MPM-Geomechanics/actions/workflows/msbuild.yml)

## Documentation

See the documentation at [MPM-Geomechanics](https://fabricix.github.io/MPM-Geomechanics/).

## Motivation

MPM-Geomechanics is a program that allows us to model the behavior of geo-materials, like soil and rock, when these materials are subjected to different initial and boundary conditions. Currently, the geo-materials are present in several areas of the society, like for example in the slopes and excavation process in mining industry activities, or in the study of risk associated to naturals disasters.

The objective of this repository is to provide a platform for developing the MPM for the study of geomechanical problems involving large deformations and distortions.

## Collaboration

If you are interested to collaborate with this project, please contact to fabricio.fernandez@ucn.cl. There are several topics for developing in this project, here there are a few of them:
- contact method for large terrain models using STL meshes [(Zhang et al., 2023)](https://www.sciencedirect.com/science/article/abs/pii/S0266352X23001684) :white_check_mark:
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

## Compiled binaries

1. Go to the [Actions page](https://github.com/fabricix/MPM-Geomechanics/actions).
2. Select the latest run of the **MSBuild** workflow for Window, or **CI** for Linux.
3. At the bottom, you will find the available artifacts under the **Artifacts** section.
4. Download the `compiled-binaries` artifact to get the compiled code.

## Documentation and Compilation

For generating documentation and compilation of the code please see the documentation at [MPM-Geomechanics](https://fabricix.github.io/MPM-Geomechanics/).

## Examples

### Slope failure
In this example an soil slope failure is simulated using an elastoplastic material:

![](/tests/slope-failure/slope-failure-test.gif)

For more details of this simulation see the input file [slope-failure.json](https://github.com/fabricix/MPM-Geomechanics/blob/main/tests/slope-failure/slope-failure.json) 

### Exponential softening model to simulate fracturing process in rock

In this example an elastoplastic body impacts over an elastic body. The fracturing process in rock masses is captured using an exponential strain softening over tensile strength in the elastoplastic material: $\sigma^t(\epsilon_p^{pleff}) = \sigma^t_{final}-(\sigma^t_{initial}-\sigma^t_{final})e^{-\eta \epsilon_p^{pleff}}$, where $\eta$ is the shape factor and $\epsilon_p^{pleff}=\sqrt{2/3\epsilon_{pij}^{pl}\epsilon_{pij}^{pl}}$ is the effective plastic strain.

#### Example 1: 
Fracturing induced by exponential softening over the tensile strength. The yellow body is subjected to an initial velocity. The withe body is elastic.

![](/tests/exponential-softening/exponential-softening-test.gif)
An elasto-plastic body impacts over an elastic body. The exponential softening used over the tensile strength, in order to reproduce the fracturing process in the body.

See [exponential-softening.json](https://github.com/fabricix/MPM-Geomechanics/blob/main/tests/exponential-softening/exponential-softening.json) input file for simulation details.

#### Example 2
In this example is tested the refinement mesh behavior. The fixed (left-bottom) and free boundary (right-up) conditions are tested too.

![](/tests/exponential-softening/refined%20mesh%20test/exponential-softening-refined.gif)

See [exponential-softening-refined.json](/tests/exponential-softening/refined%20mesh%20test/exponential-softening-refined.json) input file for simulation details.
