\page testing_manual Testing Compilation Manual

## 1. Required Programs

See [Required Programs](#required_programs).

The tests use **GoogleTest**. It is necessary to import this library by cloning the official repository into the folder `/external`. Each developer must clone this repository independently.

```
cd external
git clone https://github.com/google/googletest.git
```

## 2. How to Compile the Tests

Your directory must have the following structure:

```
mpm-geomechanics/
├─ build/
│  ├─ CMakeFiles/
│  ├─ make/
│  ├─ msbuild/
│  ├─ testing/
│  │  ├─ CMakeLists.txt
│  │  ├─ build/
```

### Windows

The simplest way to compile on windows is by using the **`.bash`** file at **`/build/Testing`** with **`MSYS2 MINGW64`** console line, just execute:

```
cd project-directory/build/Testing
./cmake-build-win.bash
```

Alternatively, you can use the following commands:

```
cd project-directory/build/Testing
cmake -G "Unix Makefiles" -B build
cmake --build build
```

These commands will generate a .exe file named `MPM-Test.exe`.

### Linux (Ubuntu/Debian)

The simplest way to compile on Linux is by using the `.bash` file at `/build/CMake`, just execute:

```
cd project-directory/build/Testing
./cmake-build-linux.bash
```

Alternatively, you can use the following commands:

```
cd project-directory/build/Testing
cmake -G "Unix Makefiles" -B build
cmake --build build
```

These commands will generate a executable file named `MPM-Test`.

