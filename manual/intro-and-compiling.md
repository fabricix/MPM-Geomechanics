\mainpage MPM-Geomechanics

# Introduction

The MPM-Geomechanics is a implementation of the Material Point Method (MPM) to simulate large strain geomechanical problems in 3D. The main objective of this program is to provide an open source platform suitable for the study and application of the MPM in various geomechanical problems were materials are subjected to extreme deformation conditions.
MPM-Geomechanics is a program that allows us to model the behavior of geo-materials, like soil and rock, when these materials are subjected to different initial and boundary conditions. Currently, the geo-materials are present in several areas of the society, like for example, in the slopes and excavation process in mining industry activities, or in the study of risk associated to naturals disasters.
This program uses the Material Point Method (MPM) to integrate the motion equation of continuum mechanics:

\f$ \frac{\partial \sigma_{i j}}{\partial x_j}+\rho b_i=\rho \ddot{u}_i \f$
that can be expressed discretely, using the nodes of an Eulerian mesh:

\f$ \dot{p}_{i I}=f_{i I}^{i n t}+f_{i I}^{e x t} \f$

where \f$ p_{i I}=\sum_p S_{I p} p_{i p}\f$ is the momentum, \f$ f_{i I}^{int}=-\sum_p \sigma_{i j p} S_{I p, j} V_p\f$ is the internal force, and \f$ f_{i I}^{e x t}=\sum_p m_p S_{I p} b_{i p}+\int_{\Gamma} \mathrm{t}_i N_I\left(x_i\right) d A\f$ is the external force at node \f$ I \f$.
The function \f$ S_{I p} \f$ and its gradient \f$ S_{I p, j} \f$ are the weighting functions of node \f$ I \f$ evaluated at the position of particle \f$ p \f$, defined by \f$ S_{I p}=\frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p\left(x_{ip}\right) N_I\left(x_{ip}\right) dV \f$ and \f$ S_{I p, j}=\frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p\left(x_{ip}\right) N_{I, j}\left(x_{ip}\right) dV \f$. 

The integration of the weight functions is performed analytically over the particle domain using linear functions for \f$ N_I\left(x_{ip}\right) \f$ and unit step functions for \f$ \chi_p\left(x_{ip}\right) \f$. 

For more details on the formulation and integration process, refer to: \ref theory_and_numerical_formulation_

# Program features

The main features of the program are:
- Three-dimensional 3D formulation
- Dynamic formulation
- Shared memory parallelization using OpenMP
- Many ways to crate bodies (polygons, particle list, pre-defined bodies)
- Several constitutive models for soil and rock materials
- Softening/hardening models to represent weakness during large deformations 
- Coupled fluid-mechanical formulation (*under development*)

# Compiled binaries

For downloading the compiled binaries
1. Go to the [Actions page](https://github.com/fabricix/MPM-Geomechanics/actions).
2. Select the latest run of the **MSBuild** workflow for Window, or **CI** for Linux.
3. At the bottom, you will find the available artifacts under the **Artifacts** section.
4. Download the `compiled-binaries` artifact to get the compiled code.

# Compilation on Windows

\note Prior to proceeding with these instructions, please consult the [Windows Required Programs](#required_programs_windows) section.

The simplest way to compile on windows is by using the **`.bash`** file at **`/build/CMake`** with **`MSYS2 MINGW64`** console line, just execute:

```
cd project-directory/build/CMake
./cmake-build.bash
```

Alternatively, you can use the following commands:

```
cd project-directory/build/CMake
cmake -G "Unix Makefiles" -B build
cmake --build build
```

# Compiling on Linux

\note Prior to proceeding with these instructions, please consult the [Linux Required Programs](#required_programs_linux) section.

The simplest way to compile on Linux is by using the `.bash` file at `/build/CMake`, just execute:

```
cd build/CMake
./cmake-build.bash
```

Alternatively, you can use the following commands:

```
cd build/CMake
cmake -G "Unix Makefiles" -B build
cmake --build build
```

# Visual Studio Solution

For compiling the code in windows you can use the Visual Studio solution file `/build/MPM-Geomechanics.sln`, and build it by pressing `Ctr+B`.
Alternatively you can compile it by using command in a *Developer Command Prompt*: 

```
msbuild MPM-Geomechanics.sln -p:Configuration=Release
```

# Make Compilation
For compile the code in a linux environment, execute the [make](https://www.gnu.org/software/make/) command into the make folder, in with is located de makefile `MPM-Geomechanics\build\make\makefile`:
 
# Documentation

The program documentation is generated using [Doxygen](https://www.doxygen.nl/index.html):

```
doxygen Doxyfile
```

The HTML generated documentation is located in `/docs/index.html`.

# Execution

In order to run simulations in several terminal, you can add the compiled code in the system `PATH`. After that, the common use is to call the program with the input file as argument: