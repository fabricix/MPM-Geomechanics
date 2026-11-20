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