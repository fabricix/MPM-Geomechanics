# Gmsh TET4 initialization

`BodyGmsh.test.cpp` uses the unit cube in
`examples/bodies/createBodyGmsh/cuboid.msh`: eight corner nodes and six
tetrahedra, each with volume 1/6 m³. No Gmsh installation is needed to run
the tests. The JSON inputs are generated in temporary directories and read
through the project's `Input` API.

The tests are included by the existing QA CMake target. After building that
target, run from the repository root:

```sh
./build/qa/MPM-Geomechanics-tests --gtest_filter='BodyGmshTest.*'
```

## Conservation checks

For a solid density of 2000 kg/m³ and zero porosity, both distributions must
produce a total mass of 2000 kg and a current volume of 1 m³:

| Distribution | Particles | Original mass | Corrected mass | Original current volume | Corrected current volume |
|---|---:|---:|---:|---:|---:|
| barycentric | 6 | 2000 kg | 2000 kg | 1 m³ | 1 m³ |
| gaussian | 24 | 500 kg | 2000 kg | 0.25 m³ | 1 m³ |

The suite covers the default JSON distribution, explicit `barycentric` and
`gaussian`, and two background cell sizes. It also checks the zero-porosity
limit using actual `ParticleMixture` instances.

For TET4 Gaussian, `Vp = V / 4` and `Mp = rho * Vp` are already per-point
quantities. Passing them directly to `addParticle` removes the additional
division by four. The barycentric path and HEX8 initialization are unchanged.

## Porous two-phase scope

The same call initializes solid mass in `ParticleMixture`. The porous test
checks only that changing the distribution preserves the sum of solid mass;
it does not assert that the mixture initialization is physically correct.

With solid density 2000 kg/m³, fluid density 1000 kg/m³, porosity 0.3 and
background cell volume 1 m³, the initialization remains:

| Distribution | Solid mass, before → after | Fluid mass | Current volume, before → after |
|---|---:|---:|---:|
| barycentric | 2000 → 2000 kg | 1800 kg | 38/17 → 38/17 m³ |
| gaussian | 500 → 2000 kg | 7200 kg | 77/17 → 92/17 m³ |

The physical reference is 1400 kg of solid, 300 kg of fluid and 1 m³.
The existing porous initialization is therefore incorrect before and after
this correction. `BodyGmsh` does not apply the solid fraction to its mass
assignment, and the fluid mass is initialized from each background-cell-sized
particle. Correcting phase fractions, fluid mass and particle dimensions is
outside this change.

`getCurrentVolume()` is derived from mass and density, so its sum is not an
independent mass check. `getInitialVolume()` uses particle sizes; with unit
background cells its sum remains 6 m³ or 24 m³. These tests do not validate
GIMP domains or time integration.
