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
executables = {
    "atomic": {"path": "MPM-Geomechanics-benchmark-atomic", "active": False},
    "main": {"path": "MPM-Geomechanics-benchmark-main", "active": False},
    "local": {"path": "MPM-Geomechanics-benchmark-local", "active": False}
}

materials_point = [150, 250, 500] # Materials points in **thousands**
threads = [1, 5, 10] # Numbers of threads

# Configuration file name generator
def config_file (p, t):
  folder = Path("benchmark")
  folder.mkdir(parents=True, exist_ok=True)
  return folder / f"config-p{p}k-t{t}"

# Run a benchmark with a specific executable and configuration file
def run_benchmark(executable_exe, executable_name, config_file):
  print(f"[INFO] Running config file: {config_file}.json with executable: {executable_exe}")
  try:
    response = subprocess.run(
      f'{executable_exe} {config_file}.json > {config_file}-{executable_name}.log',
      text=True,
      shell=True
    )
    return response
  except Exception as e:
    print(f"[ERROR] An error occurred while running the benchmark: {e}")
    print(f"[ERROR INFO] Executable: {executable_exe} | Config File: {config_file}")
    return None

# Run all the benchmarks of a specific executable
def execute_benchmarks(executable, executable_name):
  print(f"[INFO] Starting benchmarks for executable: {executable_name}")
  for p in materials_point:
    for t in threads:
      run_benchmark(executable, executable_name, config_file(p, t))
  print(f"[INFO] Completed benchmarks for executable: {executable_name}\n")

def create_configuration_files():
    for p in materials_point:
        for t in threads:
            json_template["num_particles"] = p * 1000
            json_template["num_threads"] = t
            with open(f"{config_file(p, t)}.json", "w") as f:
                json.dump(json_template, f, indent=4)

def adjust_paths():
    if sys.platform == "win32":
        for exe in executables.keys():
            executables[exe]["path"] = f"{executables[exe]['path']}.exe"
    if sys.platform == "linux":
        script_dir = os.path.dirname(os.path.abspath(__file__))
        for exe in executables.keys():
            executables[exe]["path"] = os.path.join(script_dir, executables[exe]["path"])

def read_parameters_from_console():
    if len(sys.argv) > 1:

      for parameter in sys.argv[1:]:
        split_parameter = parameter.split(":")
        exe_name = split_parameter[0]

        if len(split_parameter) > 1:
            path = split_parameter[1]
        elif exe_name in executables.keys():
            path = executables[exe_name]["path"]

        if exe_name in executables.keys():
          executables[exe_name]["active"] = True
          executables[exe_name]["path"] = path
          print(f"[INFO] Enabled executable for {exe_name} | path: {path}")
        else: 
          print(f"[INFO] The parameter provided is not valid: {parameter}")

    else: 
      print(f"[INFO] No parameters found")
      for executable in executables.values():
        executable["active"] = True

def start_benchmarks():

    # Start time measurement
    start_time = time.time()

    if executables["main"]["active"]:
      thread01 = threading.Thread(target=execute_benchmarks, args=(executables["main"]["path"], "main"))
      thread01.start()

    if executables["atomic"]["active"]:
        thread02 = threading.Thread(target=execute_benchmarks, args=(executables["atomic"]["path"], "atomic"))
        thread02.start()

    if executables["local"]["active"]:
        thread03 = threading.Thread(target=execute_benchmarks, args=(executables["local"]["path"], "local"))
        thread03.start()

    if executables["main"]["active"]:
        thread01.join()

    if executables["atomic"]["active"]:
        thread02.join()

    if executables["local"]["active"]:
        thread03.join()

    # End time measurement
    end_time = time.time()
    elapsed_time = end_time - start_time
    print(f"[INFO] Total elapsed time: {elapsed_time:.2f} seconds")

# Main function
def main():

    # Adjust executable paths
    adjust_paths()

    # Create configuration files
    create_configuration_files()

    # Read parameters from the console
    read_parameters_from_console()

    # Start benchmarking
    start_benchmarks()

# Start benchmarking
main()