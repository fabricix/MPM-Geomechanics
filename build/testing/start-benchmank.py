import subprocess
import threading
import json
import time
import sys
from pathlib import Path
import os

# Json template for testing
json_template = {
  "num_particles": 150,
  "num_threads": 1,
  "particle_size": [1.0, 1.0, 1.0],
  "particle_mass": 1.0,
  "cell_dimension": [1.0, 1.0, 1.0],
  "num_cells": [100, 100, 100],
  "random_seed": 42
}

# Executable file names
executables = {}

# Materials points in **thousands**
materials_point = [150, 250, 500] 

# Numbers of threads
threads = [1, 5, 10] 

CONFIG_FOLDER = "benchmark/configuration-files"
LOGS_FOLDER = "logs"

# Configuration file name generator
def config_file (p, t):
  return f"config-p{p}k-t{t}"

def get_logs_path(name):
  return f"benchmark/{name}"

def get_executable_name(executable_path):
  return executable_path.split('/')[-1]

def create_folders():
  Path("benchmark").mkdir(parents=True, exist_ok=True)
  Path(f"{CONFIG_FOLDER}").mkdir(parents=True, exist_ok=True)
  for executable_path in executables.values():
    executable_name = get_executable_name(executable_path)
    Path(f"{get_logs_path(executable_name)}").mkdir(parents=True, exist_ok=True)

# Run a benchmark with a specific executable and configuration file
def run_benchmark(executable_path, name, config_file):
  print(f"[INFO] [{name}] Running config file: {config_file}.json with executable: {get_executable_name(executable_path)}")
  try:
    response = subprocess.run(
      f'{executable_path} {CONFIG_FOLDER}/{config_file}.json > {get_logs_path(get_executable_name(executable_path))}/{config_file}-{name}.log',
      text=True,
      shell=True  
    )
    return response
  except Exception as e:
    print(f"[ERROR] An error occurred while running the benchmark: {e}")
    print(f"[ERROR] Executable: {executable_path} | Config File: {config_file}")
    sys.exit(-1)

# Run all the benchmarks of a specific executable
def execute_benchmarks(executable_path, name):
  print(f"[INFO] [{name}] Starting benchmarks")
  for p in materials_point:
    for t in threads:
      run_benchmark(executable_path, name, config_file(p, t))
  print(f"[INFO] [{name}] Completed benchmarks\n")

def create_configuration_files():
    create_folders()
    for p in materials_point:
        for t in threads:
            json_template["num_particles"] = p * 1000
            json_template["num_threads"] = t
            with open(f"{CONFIG_FOLDER}/{config_file(p, t)}.json", "w") as f:
                json.dump(json_template, f, indent=4)

def read_parameters_from_console():
    if len(sys.argv) > 1:

      for parameter in sys.argv[1:]:
        split_parameter = parameter.split(":")
        if len(split_parameter) > 1:
        
          exe_name = split_parameter[0]
          path = split_parameter[1].split(".exe")[0]
          executables[exe_name] = path
          
    else: 
      print(f"[ERROR] No parameters found. Please provide at least one executable in the format: <name>:<path-to-executable>")
      sys.exit(-1)

    # Adjust executable paths
    if sys.platform == "win32":
        print("[INFO] Windows platform detected")
        for exe in executables.keys():
            if not os.path.exists(f"{executables[exe]}.exe"):
                print(f"[ERROR] Executable not found for {exe}: {executables[exe]}")
                sys.exit(-1)

    if sys.platform == "linux" or sys.platform == "cygwin":
        print("[INFO] Linux platform detected")
        script_dir = os.path.dirname(os.path.abspath(__file__))
        for exe in executables.keys():
            executables[exe] = os.path.join(script_dir, executables[exe])
            if not os.path.exists(executables[exe]):
                print(f"[ERROR] Executable not found for {exe}: {executables[exe]}")
                sys.exit(-1)

def start_benchmarks():

    # Start time measurement
    start_time = time.time()

    if executables.values():
      print(f"[INFO] Starting benchmarks with executables: {', '.join(executables.keys())}")

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
    print(f"[INFO] Total elapsed time: {elapsed_time:.2f} seconds")

# Main function
def main():

    # Read parameters from the console
    read_parameters_from_console()

    # Create configuration files 
    create_configuration_files()

    # Start benchmarking
    start_benchmarks()

    return 0

# Start benchmarking process
main()