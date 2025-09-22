\page testing_manual Testing Compilation and Benchmarking

\section testing_manual_howto_compile 1. How to Compile

\note Prior to proceeding with these instructions, please consult the [Required Programs](#required_programs) section.

The tests use **GoogleTest**. It is necessary to import this library by cloning the official repository into the folder `/external`. Each developer must clone this repository independently.

```
cd external
git clone https://github.com/google/googletest.git
```

The simplest way to compile on Windows and Linux is by using the `.bash` file at `/build/qa` with `MSYS2 MINGW64` console line, simply execute the following command in the directory `MPM-Geomechanics/build/qa`:
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

These commands will generate two executables: `MPM-Geomechanics-tests` and `MPM-Geomechanics-benchmark`.

- `MPM-Geomechanics-tests`: Run testing using GoogleTest. All files ending with `.test.cpp` are testing files, you can find them in the directory `qa/tests`.

- `MPM-Geomechanics-benchmark`: Run benchmark using GoogleTest. All files ending with `.benchmark.cpp` are performance files. You can find them in the directory `qa/benchmark`.

\section testing_manual_howto How does benchmarking work?

\note If you are using **Windows OS**, make sure to use the **MINGW64 command-line console**.

\subsection testing_manual_howto_benchmark Executable: MPM-Geomechanics-benchmark

To run the benchmark correctly, a JSON file named `benchmark-configuration.json` is required. This file allows the user to specify values for each test. If the file does not exist or a value is missing, a default value will be used.

The executable `MPM-Geomechanics-benchmark` allows the following command-line arguments:

* `<directory>`: Specifies which file should be used to run the benchmark. If no file is specified, the program will use `benchmark-configuration.json` located in the same directory as the executable. Example: `MPM-Geomechanics-benchmark configuration-file.json`

The executable `MPM-Geomechanics-benchmark` allows the following command-line flags:

* `--log`: Shows more information about missing keys in the `benchmark-configuration.json` file

\subsection testing_manual_howto_multibenchmark Executable: start-multi-benchmark.py

The performance test can also be executed using the `start-multi-benchmark.py` script, which allows running benchmarks with one or more executables downloaded as artifacts from GitHub and stores the log results in separate folders. Each executable is automatically downloaded from GitHub as an artifact, using an ID specified in `start-multi-benchmark-configuration.json`. Additionally, the benchmark configuration (number of martial points and number of threads) can be defined in the same file. 

Example of a `start-multi-benchmark-configuration.json` file:

```
{
	"executables": {
		"win_benchmark_executable": "MPM-Geomechanics-benchmark.exe",  -> An executable located at build/qa/MPM-Geomechanics-benchmark.exe
    	"win_github_id": "17847226555"  -> An artifact ID that represent it. can check out if it exists via gh run view <ID>
 	},
	"parameters": {
		"particles": [ 15000, 25000, 50000],  -> material point to test
		"threads": [1, 5, 10]  -> number of threads to test
	}
}
```

As you can see, this setup will run 3x3 (particles x threads) tests for **win_benchmark_executable** and **win_github_id**, totaling 18 tests (3x3x2)

The executable `start-multi-benchmark.py` allows the following command-line flags:

* `--clear`: Removes the **benchmark folder** (if it exists) before executing the performance tests.
* `--cache`: Uses the previously created cache instead of the `start-multi-benchmark-configuration.json` file.