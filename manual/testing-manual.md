\page testing_manual Testing Compilation and Benchmarking

\section testing_manual_howto 1. How to Compile

\note Prior to proceeding with these instructions, please consult the [Required Programs](#required_programs) section.

The tests use **GoogleTest**. It is necessary to import this library by cloning the official repository into the folder **`/external`**. Each developer must clone this repository independently.

```
cd external
git clone https://github.com/google/googletest.git
```

The simplest way to compile on Windows and Linux is by using the **`.bash`** file at **`/build/testing`** with **`MSYS2 MINGW64`** console line, simply execute the following command in the directory **`MPM-Geomechanics/build/testing`**:
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

These commands will generate two executables: **`MPM-Geomechanics-tests`** and **`MPM-Geomechanics-benchmark`**.

- **`MPM-Geomechanics-tests`**: Run testing using GoogleTest. All files ending with **`.test.cpp`** are testing files, you can find them in the directory **`qa/tests`**.

- **`MPM-Geomechanics-benchmark`**: Run benchmark using GoogleTest. All files ending with **`.benchmark.cpp`** are performance files. You can find them in the directory **`qa/benchmark`**.

\section testing_manual_benchmark How does benchmarking work?

To correctly execute the benchmarking, a JSON file called **`benchmark-configuration.json`** is needed, which allows the user to specify the values for each test. If the file does not exist or if a value is not referenced in the JSON file, a default value will be used.

The executable **`MPM-Geomechanics-benchmark`** allows the following command-line arguments:

* **`<directory>`**: Indicates which file should be used to run the benchmarking. If no file is specified by the user, it will use a JSON file named **`benchmark-configuration.json`** located in the same directory as the executable. Example: **`MPM-Geomechanics-benchmark configuration-file.json`**

The executable **`MPM-Geomechanics-benchmark`** allows the following command-line flags:

* **`--log`**: Shows more information about missing keys in the **`benchmark-configuration.json`** file

The performance test can also be executed using the **`start-multi-benchmark.py`** script, which supports running benchmarks with one or more executables downloaded as artifacts from GitHub, storing the log results in separate folders. Each executable is automatically downloaded from GitHub as an artifact. For this, an ID must be specified in the **`start-multi-benchmark-configuration.json`** file. Additionally, the benchmark configuration (number of martial points and number of threads) can be defined in the same file. 

\note If you are using windows OS, make sure you are using the MINGW64 console-line. Check [Required Programs](#required_programs_windows) for more information.

The executable **`start-multi-benchmark.py`** allows the following command-line flags:

* **`--clear`**: Removes the **`benchmark`** folder before executing the performance tests.