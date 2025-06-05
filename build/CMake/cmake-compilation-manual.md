## Compilation with CMake

### Windows Instructions

#### Required Programs

| Program          | Installation |
| ---------------- | ------------ |
| **CMake**        | via Winget   |
| **MinGW32-make** | via MSYS2    |
| **GCC**          | via MSYS2    |
| **G++**          | via MSYS2    |

#### Compilation with Windows

The simplest way to compile on windows is by using the `.bat` file at `/build/CMake`, just execute:

```
cd build/CMake
./cmake-build-win.bat
```

Alternatively, you can use the following commands:

```
cd build/CMake
cmake -G "MinGW Makefiles" -B build
cmake --build build
```

### Linux Instructions

#### Required Programs

| Program   | Installation |
| --------- | ------------ |
| **CMake** | via apt      |
| **Make**  | via apt      |
| **GCC**   | via apt      |
| **G++**   | via apt      |

#### Compilation with Linux

The simplest way to compile on Linux is by using the `.bash` file at `/build/CMake`, just execute:

```
cd build/CMake
./cmake-build-linux.bash
```

Alternatively, you can use the following commands:

```
cd build/CMake
cmake -G "Unix Makefiles" -B build
cmake --build build
```
