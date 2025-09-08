\page testing_manual Testing Compilation and Benchmarking

\section testing_manual_howto 1. How to Compile

\note Prior to proceeding with these instructions, please consult the [Required Programs](#required_programs) section.

The tests use **GoogleTest**. It is necessary to import this library by cloning the official repository into the folder **`/external`**. Each developer must clone this repository independently.

```
cd external
git clone https://github.com/google/googletest.git
```

The simplest way to compile on Windows and Linux is by using the **`.bash`** file at **`/build/Testing`** with **`MSYS2 MINGW64`** console line, simply execute the following command in the directory **`MPM-Geomechanics/build/testing`**:
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

- **`MPM-Geomechanics-Test`**: Run testing using GoogleTest. All files ending with **`.test.cpp`** are testing files, you can find them in the directory **`tests/numerical-verification`**.

- **`MPM-Geomechanics-Benchmark`**: Run benchmark using GoogleTest. All files ending with **`.performance.test`** are performance files. You can find them in the directory **`tests/performance`**.

\section testing_manual_benchmark How does benchmarking work?

To correctly execute the benchmarking, a JSON file called `test-configuration.json` is needed, which allows the user to specify the values for each test. If the file does not exist or if a value is not referenced in the JSON file, a default value will be used.

The executable **`MPM-Geomechanics-Benchmark`** allows the following command-line arguments:

* **`<directory>`**: Indicates which file should be used to run the benchmarking. If no file is specified by the user, it will use a JSON file named **`test-configuration.json`** located in the same directory as the executable. Example: **`MPM-Geomechanics-Benchmark configuration-file.json`**