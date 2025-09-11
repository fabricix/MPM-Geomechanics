import shutil
import subprocess
import threading
import json
import time
import sys
from pathlib import Path
import os

# Json template for testing
json_template = {
    "global":
    {  
        "numParticles": 150,
        "numThreads": 1,
        "particleSize": [1.0, 1.0, 1.0],
        "particleMass": 1.0,
        "cellDimension": [1.0, 1.0, 1.0],
        "numCells": [100, 100, 100],
        "randomSeed": 42
    }
}

# Executable file names
executables = {"current": "MPM-Geomechanics-benchmark"}

# Materials points in **thousands**
materials_point = [150, 250, 500] 

# Numbers of threads
threads = [1, 5, 10] 

CONFIG_FOLDER = "configuration-files"
BENCHMARK_FOLDER = "benchmark"
ARTIFACT_FOLDER = "artifact"
LOGS_FOLDER = "logs"

# Configuration file name generator
def config_file (p, t):
    return f"config-p{p}-t{t}"

def get_logs_path(name):
    return f"{BENCHMARK_FOLDER}/{LOGS_FOLDER}/{name}"

def get_executable_name(executable_path):
    return executable_path.split('/')[-1]

def create_folders():
    print("\n> Creating configuration files")
    Path(f"{BENCHMARK_FOLDER}").mkdir(parents=True, exist_ok=True)
    Path(f"{BENCHMARK_FOLDER}/{CONFIG_FOLDER}").mkdir(parents=True, exist_ok=True)
    Path(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}").mkdir(parents=True, exist_ok=True)
    Path(f"{BENCHMARK_FOLDER}/{LOGS_FOLDER}").mkdir(parents=True, exist_ok=True)
    for executable_path in executables.values():
      executable_name = get_executable_name(executable_path)
      Path(f"{get_logs_path(executable_name)}").mkdir(parents=True, exist_ok=True)

# Run a benchmark with a specific executable and configuration file
def run_benchmark(executable_path, name, config_file):
    print(f"----> [{name}] Running config file: {config_file}.json with executable: {get_executable_name(executable_path)}")
    return
    try:
        response = subprocess.run(
        f'{executable_path} {CONFIG_FOLDER}/{config_file}.json > {get_logs_path(get_executable_name(executable_path))}/{config_file}-{name}.log',
        text=True,
        shell=True  )
        return response
    except Exception as e:
        print(f"--> [ERROR] An error occurred while running the benchmark: {e}")
        print(f"--> [ERROR] Executable: {executable_path} | Config File: {config_file}")
        sys.exit(-1)

# Run all the benchmarks of a specific executable
def execute_benchmarks(executable_path, name):
  print(f"--> [{name}] Starting benchmarks")
  for p in materials_point:
    for t in threads:
      run_benchmark(executable_path, name, config_file(p, t))
  print(f"--> [{name}] Completed benchmarks\n")

def create_configuration_files():

    for p in materials_point:
        for t in threads:
            json_template["global"]["numParticles"] = p
            json_template["global"]["numThreads"] = t
            with open(f"{BENCHMARK_FOLDER}/{CONFIG_FOLDER}/{config_file(p, t)}.json", "w") as f:
                json.dump(json_template, f, indent=4)
    print(f"--> Configuration files created in {BENCHMARK_FOLDER}/{CONFIG_FOLDER}")

def read_configuration():
    global materials_point
    global threads
    global executables

    print("\n> Reading configuration from 'start-benchmark-configuration.json'")

    json_configuration = {}
    with open ("start-benchmark-configuration.json", "r") as f:
        json_configuration = json.load(f)
        
    if "executables" in json_configuration:
        print("--> Custom executables found in configuration file") 
        executables.clear()
        executables_list = json_configuration["executables"]
        for name, path in executables_list.items():
            if not path.isdigit():
                print(f"----> Custom executable path provided for [{name}]: {path}")
                executables[name] = path
                continue
            else:
                print(f"----> Custom executable ID provided for [{name}]: {path}")

        # Delete ARTIFACT folder if exists
        if Path(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}").exists():
            print(f"----> Deleting existing ARTIFACT folder: {ARTIFACT_FOLDER}")
            shutil.rmtree(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}")

        # Download executables from GitHub Actions using 'gh' CLI
        for name, path in executables_list.items():
            if path.isdigit():
                print(f"----> Checking GitHub for run ID [{path}] for executable [{name}]...")
                try:
                    print(f"------> Checking if the run ID [{path}] exists...")
                    subprocess.run(f"gh run view {path}", shell=True, text=True, capture_output=True, check=True)
                    print(f"------> ID [{path}] exists.")
                except Exception as e:
                    print(f"------> [ERROR] An error occurred while checking the run ID [{path}]")
                    print(f"------> [ERROR] Please check if the run ID [{path}] exists in GitHub Actions")
                    print(f"------> [ERROR] {e}")
                    sys.exit(-1)
                    
                try:
                    print(f"------> Downloading the executable for [{name}] to {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}...")
                    subprocess.run(f"gh run download {path} -D {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}", shell=True, text=True, capture_output=True, check=True)
                    print(f"------> Download completed for [{name}]")
                except Exception as e:
                    print(f"--> [ERROR] An error occurred while downloading executable for [{name}]")
                    print(f"--> [ERROR] Please check if the run ID [{path}] exists in GitHub Actions")
                    print(f"--> [ERROR] {e}")
                    sys.exit(-1)

                # Move the executable to a folder with the name of the executable
                origin = f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/MPM-Geomechanics-benchmark-linux/MPM-Geomechanics-benchmark"
                destination = f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}/MPM-Geomechanics-benchmark"
                Path(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}").mkdir(parents=True, exist_ok=True)
                shutil.move(origin, destination)
                shutil.rmtree(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/MPM-Geomechanics-benchmark-linux")
                    
        # read all ARTIFACT in the ARTIFACT folder
        ARTIFACT_files = os.listdir(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}")
        if len(ARTIFACT_files) == 0:
            print(f"----> [ERROR] No ARTIFACT found in {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}")
            sys.exit(-1)
            
        # get the first file with executable extension
        for file in ARTIFACT_files:
            if sys.platform == "win32" and file.endswith(".exe"):
                executables[name] = f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{file}/MPM-Geomechanics-benchmark"
                executables[name] = executables[name].replace("\\", "/")
                print(f"------> Executable for [{name}] found: {executables[name]} via an artifact")
                break
            if (sys.platform == "linux" or sys.platform == "cygwin") and not file.endswith(".exe"):
                executables[name] = f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{file}/MPM-Geomechanics-benchmark"
                executables[name] = executables[name].replace("\\", "/")
                print(f"------> Executable for [{name}] found: {executables[name]} via an artifact")
                break

    if "parameters" in json_configuration:
        print("--> Custom parameters found in configuration file")
        executables_parameters = json_configuration["parameters"]
        materials_point = executables_parameters["particles"] if "particles" in executables_parameters else materials_point
        threads = executables_parameters["threads"] if "threads" in executables_parameters else threads
        print(f"----> Materials points: {materials_point}")
        print(f"----> Threads: {threads}")

    # Adjust executable paths
    if sys.platform == "win32":
        for exe in executables.keys():
            if not os.path.exists(f"{executables[exe]}.exe"):
                print(f"--> [ERROR] Executable not found for [{exe}]: {executables[exe]}")
                sys.exit(-1)

    if sys.platform == "linux" or sys.platform == "cygwin":
        script_dir = os.path.dirname(os.path.abspath(__file__))
        for exe in executables.keys():
            executables[exe] = os.path.join(script_dir, executables[exe])
            if not os.path.exists(executables[exe]):
                print(f"--> [ERROR] Executable not found for [{exe}]: {executables[exe]}")
                sys.exit(-1)


def start_benchmarks():

    # Start time measurement
    start_time = time.time()

    if executables.values():
      print(f"\n>  Starting benchmarks with executables: {', '.join(executables.keys())}")

    # Create and start threads for each executable
    threads = []
    for name, executable_path in executables.items():
      thread = threading.Thread(target=execute_benchmarks, args=(executable_path, name))
      thread.start()
      threads.append(thread)
      break

    for thread in threads:
      thread.join()
      break

    # End time measurement
    end_time = time.time()
    elapsed_time = end_time - start_time
    print(f"Total elapsed time: {elapsed_time:.2f} seconds")

# Main function
def main():

    # Create necessary folders
    create_folders()

    # Read parameters from the console
    read_configuration()

    # Create configuration files 
    create_configuration_files()

    # Start benchmarking
    start_benchmarks()

    return 0

# Start benchmarking process
main()