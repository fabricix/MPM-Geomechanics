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

def create_folders():
    # get parameters from the console line
    arg = " ".join(sys.argv[1:])
    if arg == "--clean":
        if Path(f"{BENCHMARK_FOLDER}").exists():
            response = input(f"> The folder {BENCHMARK_FOLDER} already exists. Do you want to delete it? (y/n): ")
            if response.lower() == "y":
                print(f"--> Deleting existing BENCHMARK folder")
                shutil.rmtree(f"{BENCHMARK_FOLDER}")
                print(f"--> Folder {BENCHMARK_FOLDER} deleted")

    print("\n> Creating folders")
    Path(f"{BENCHMARK_FOLDER}").mkdir(parents=True, exist_ok=True)
    Path(f"{BENCHMARK_FOLDER}/{CONFIG_FOLDER}").mkdir(parents=True, exist_ok=True)
    Path(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}").mkdir(parents=True, exist_ok=True)
    Path(f"{BENCHMARK_FOLDER}/{LOGS_FOLDER}").mkdir(parents=True, exist_ok=True)
    print(f"--> Folders created in {BENCHMARK_FOLDER}")

def create_log_folder():
    for name in executables.keys():
        Path(f"{BENCHMARK_FOLDER}/{LOGS_FOLDER}/{name}").mkdir(parents=True, exist_ok=True)

# Run a benchmark with a specific executable and configuration file
def run_benchmark(executable_path, name, config_file):
    print(f"----> [{name}] Running config file: {config_file}.json with executable: {executable_path}")
    try:
        log_reference = Path(f"{BENCHMARK_FOLDER}/{LOGS_FOLDER}/{name}/{config_file}-{name}.log")
        config_path = Path(f"{BENCHMARK_FOLDER}/{CONFIG_FOLDER}/{config_file}.json")
        with open(log_reference, "w") as log_file:
            subprocess.run([executable_path, str(config_path)], stdout=log_file, stderr=subprocess.STDOUT, check=True)

    except Exception as e:
        print(f"----> [ERROR] An error occurred while running the benchmark: {e}")
        print(f"----> [ERROR] Executable: {executable_path} | Config File: {config_file}")

# Run all the benchmarks of a specific executable
def execute_benchmarks(executable_path, name):
  print(f"--> Starting benchmarks: [{name}] ")
  for p in materials_point:
    for t in threads:
      run_benchmark(executable_path, name, config_file(p, t))
  print(f"--> Completed benchmarks: [{name}]\n")

def create_configuration_files():
    print(f"\n> Creating configuration files")
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
    json_configuration = {}

    print("\n> Reading configuration from 'start-benchmark-configuration.json'")
    with open ("start-benchmark-configuration.json", "r") as f:
        json_configuration = json.load(f)
        
    if "executables" in json_configuration:
        print("--> Custom executables found in configuration file") 
        executables.clear()
        executables_list = json_configuration["executables"]
        for name, path in executables_list.items():
            if path.isdigit():
                print(f"----> Custom executable ID provided for [{name}]: {path}")
            else:
                if not Path(path).is_file():
                    print(f"----> [ERROR] The provided path for [{name}] is not a valid file: {path}")
                    print(f"----> [ERROR] Please check the start-benchmark-configuration.json file")
                    raise FileNotFoundError(f"The provided path for [{name}] is not a valid file: {path}")
                print(f"----> Custom executable path provided for [{name}]: {path}")
            executables[name] = path

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
                    
                try:
                    print(f"------> Downloading the executable for [{name}] to {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}...")
                    subprocess.run(f"gh run download {path} -D {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}", shell=True, text=True, capture_output=True, check=True)
                    print(f"------> Download completed for [{name}]")
                except Exception as e:
                    print(f"----> [ERROR] An error occurred while downloading executable for [{name}]")
                    print(f"----> [ERROR] Please check if the run ID [{path}] exists in GitHub Actions")
                    print(f"----> [ERROR] {e}")

                artifact_folder_name = ""
                if sys.platform == "win32" or sys.platform == "cygwin":
                    artifact_folder_name = "MPM-Geomechanics-benchmark-windows"

                if sys.platform == "linux":
                    artifact_folder_name = "MPM-Geomechanics-benchmark-linux"

                # Move the executable to a folder with the name of the executable
                origin = f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{artifact_folder_name}/MPM-Geomechanics-benchmark.exe"
                destination = f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}/MPM-Geomechanics-benchmark.exe"
                Path(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}").mkdir(parents=True, exist_ok=True)
                shutil.move(origin, destination)
                shutil.rmtree(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/MPM-Geomechanics-benchmark-windows")
                executables[name] = destination

    if "parameters" in json_configuration:
        print("\n--> Custom parameters found in configuration file")
        executables_parameters = json_configuration["parameters"]
        materials_point = executables_parameters["particles"] if "particles" in executables_parameters else materials_point
        threads = executables_parameters["threads"] if "threads" in executables_parameters else threads
        print(f"----> Materials points: {materials_point}")
        print(f"----> Threads: {threads}")

    if executables.values():
        print("\n--> Executables to be used:")
        for name, path in executables.items():
            print(f"----> [{name}]: {path}")

    print(f"\n--> All parameters read successfully")

def start_benchmarks():
    print("\n> Starting benchmarks")

    # Start time measurement
    start_time = time.time()

    # Create and start threads for each executable
    threads = []
    for name, executable_path in executables.items():
      thread = threading.Thread(target=execute_benchmarks, args=(executable_path, name))
      thread.start()
      threads.append(thread)

    for thread in threads:
      thread.join()

    # End time measurement
    end_time = time.time()
    elapsed_time = end_time - start_time
    print("--> All benchmarks completed")
    print(f"--> Check {BENCHMARK_FOLDER}/{LOGS_FOLDER} for logs")
    print(f"--> Total elapsed time: {elapsed_time:.2f} seconds")

# Main function
def main():
    try: 
        # Create necessary folders
        create_folders()

        # Read parameters from the console
        read_configuration()

        # Create log folders
        create_log_folder()

        # Create configuration files 
        create_configuration_files()

        # Start benchmarking
        start_benchmarks()

        return 0
    except:
        print(f"--> [ERROR] An error occurred during the benchmarking process")
        return -1

# Start benchmarking process
main()