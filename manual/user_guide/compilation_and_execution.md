# Compilation and Execution {#code_execution}

## Build Commands {#build_commands}

**MPM-Geomechanics** uses CMake for out-of-source builds. From the repository root, run:

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

*(On Windows under MSYS2 MINGW64, use `cmake -G "MinGW Makefiles" ..` followed by `mingw32-make -j4`).*

---

## Compiled Binaries {#compiled_binaries}

After a successful build, the following executables are generated inside `build/` (or the configured binary output folder):

* `mpm-geomechanics`: Main simulation engine executable.
* `mpm-tests` (if QA/testing is enabled): GoogleTest suite for unit and regression testing.

---

## Program Execution {#program_execution}

Simulations are configured via a `.json` input file containing mesh geometry, boundary conditions, material constitutive parameters, body definitions, and solver settings. To run a simulation:

```bash
./build/mpm-geomechanics path/to/input_model.json
```

### Saving and Loading Simulation States
For multi-stage analyses (such as generating an initial geostatic stress field via quasi-static damping before applying dynamic or undrained loads), the stress and particle state can be saved and reloaded in the JSON input file:

**Save state:**
```json
"save_state": true,
"save_state_file": "my_state_to_save.json"
```

**Load state:**
```json
"load_state": true,
"load_state_file": "my_state_to_load.json"
```

### Slave-Master Contact Configuration
To enable multi-body frictional contact in the `.json` input file, configure the `contact_manager` and define each master-slave pair inside the `contact` array (setting `real_distance_correction_coefficient` to `0` disables the gap correction):

```json
"contact_manager": {
  "active": true,
  "real_distance_correction_coefficient": 0.5
},
"contact": [
  {
    "id": 1,
    "friction": 0.2,
    "master_id": 1,
    "slave_id": 2,
    "normal_type": "master"
  }
]
```