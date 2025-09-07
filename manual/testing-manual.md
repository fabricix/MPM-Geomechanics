\page testing_manual Testing Compilation and Benchmarking Manual

\section testing_manual_required_programs 1. Required Programs

\note Prior to proceeding with these instructions, please consult the [Required Programs](#required_programs) section.

The tests use **GoogleTest**. It is necessary to import this library by cloning the official repository into the folder **`/external`**. Each developer must clone this repository independently.

```
cd external
git clone https://github.com/google/googletest.git
```

\section testing_manual_howto 2. How to Compile

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

The simplest way to compile on windows and Linux is by using the **`.bash`** file at **`/build/Testing`** with **`MSYS2 MINGW64`** console line, just execute the following command in the directory **`project-directory/build/Testing`**:
```
./cmake-build.bash
```

Alternatively, you can use the following commands:
```
cmake -G "Unix Makefiles" -B build
```

```
cmake --build build
```

These commands will generate two executables: **`MPM-Geomechanics-Test`** and **`MPM-Geomechanics-Benchmark`**.

- **`MPM-Geomechanics-Test`**: Run testing using GoogleTest. All files ended with **`.test.cpp`** are testing files, you can find them in the directory **`tests/numerical-verification`**.

- **`MPM-Geomechanics-Benchmark`**: Run benchmark using GoogleTest. All files ended with **`.performance.test`** are performance files. You can find them in the directory **`tests/performance`**.