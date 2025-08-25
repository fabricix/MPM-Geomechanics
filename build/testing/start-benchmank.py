import subprocess
import threading
import json
import time

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
    "atomic": "MPM-Geomechanics-benchmark-atomic.exe",
    "main": "MPM-Geomechanics-benchmark-main.exe",
    "local": "MPM-Geomechanics-benchmark-local.exe"
}

materials_point = [150, 250, 500] # Materials points in **thousands**
threads = [1, 5, 10] # Numbers of threads

# Configuration file name generator
def config_file (p, t):
  return f"config-p{p}k-t{t}"

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

# Main function
def main():

    # Create configuration files
    for p in materials_point:
      for t in threads:
        json_template["num_particles"] = p * 1000
        json_template["num_threads"] = t
        with open(f"{config_file(p, t)}.json", "w") as f:
          json.dump(json_template, f, indent=4)

    # Start time measurement
    start_time = time.time()

    thread01 = threading.Thread(target=execute_benchmarks, args=(executables["main"], "main"))
    thread01.start()

    thread02 = threading.Thread(target=execute_benchmarks, args=(executables["atomic"], "atomic"))
    thread02.start()

    thread03 = threading.Thread(target=execute_benchmarks, args=(executables["local"], "local"))
    thread03.start()

    thread01.join()
    thread02.join()
    thread03.join()

    # End time measurement
    end_time = time.time()
    elapsed_time = end_time - start_time
    print(f"[INFO] Total elapsed time: {elapsed_time:.2f} seconds")

# Start benchmarking
main()