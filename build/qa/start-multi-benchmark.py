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
BENCHMARK_CONFIGURATION_FILE_NAME = "start-multi-benchmark-configuration.json"
CACHE_FILE_NAME = "start-multi-benchmark-configuration-cached.json"

def read_flags():
    global clean_flag
    global cache_flag
    clean_flag = False
    cache_flag = False
    print("")
    arg = " ".join(sys.argv[1:])
    if arg == "--clean":
        print("> Clean flag detected (--clean)")
        clean_flag = True
    if arg == "--cache":
        print("> Cache flag detected (--cache)")
        cache_flag = True

# Configuration file name generator
def config_file (p, t):
    return f"config-p{p}-t{t}"

def create_folders():
    # get parameters from the console line
    if clean_flag:
        if Path(f"{BENCHMARK_FOLDER}").exists():
            response = input(f"\n> The folder {BENCHMARK_FOLDER} already exists. Do you want to delete it? (y/n): ")
            if response.lower() == "y":
                print(f"--> Deleting existing {BENCHMARK_FOLDER} folder")
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

        exe_abs = os.path.abspath(executable_path)
        cfg_abs = os.path.abspath(config_path)

        Path(log_reference.parent).mkdir(parents=True, exist_ok=True)

        with open(log_reference, "w") as log_file:
            subprocess.run([exe_abs, str(cfg_abs)],
                           stdout=log_file,
                           stderr=subprocess.STDOUT,
                           check=True,
                           cwd=os.path.dirname(exe_abs))
    except Exception as e:
        print(f"----> [ERROR] An error occurred while running the benchmark: {e}")
        print(f"----> [ERROR] Executable: {executable_path} | name: {name} | Config File: {config_file}")

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
    id_found = False

    if cache_flag and Path(f"{BENCHMARK_FOLDER}/{CACHE_FILE_NAME}").is_file():
        print(f"\n> Reading cached configuration from {BENCHMARK_FOLDER}/{CACHE_FILE_NAME}")
        with open(f"{BENCHMARK_FOLDER}/{CACHE_FILE_NAME}", "r") as f:
            json_configuration = json.load(f)
        if "executables" in json_configuration:
            print("--> Cached executables found")
            executables = json_configuration["executables"]
            print("--> Cached executables loaded")
        if "parameters" in json_configuration:
            print("--> Cached parameters found")
            executables_parameters = json_configuration["parameters"]
            materials_point = executables_parameters["particles"] if "particles" in executables_parameters else materials_point
            threads = executables_parameters["threads"] if "threads" in executables_parameters else threads
            print(f"----> Materials points: {materials_point}")
            print(f"----> Threads: {threads}")
        print(f"--> All parameters read successfully from cache")
        return
    elif cache_flag:
        print(f"--> [ERROR] Cached configuration file {BENCHMARK_FOLDER}/{CACHE_FILE_NAME} not found. Reading from {BENCHMARK_CONFIGURATION_FILE_NAME}")
        raise

    print(f"\n> Reading configuration from {BENCHMARK_CONFIGURATION_FILE_NAME}")
    if not Path(BENCHMARK_CONFIGURATION_FILE_NAME).is_file():
        print(f"--> [WARNING] Configuration file '{BENCHMARK_CONFIGURATION_FILE_NAME}' not found. Using default parameters.")
        return

    with open(BENCHMARK_CONFIGURATION_FILE_NAME, "r") as f:
        json_configuration = json.load(f)

    # Read executables from configuration file
    if "executables" in json_configuration:
        print("--> Custom executables found in configuration file") 
        executables.clear()
        executables_list = json_configuration["executables"]
        for name, path in executables_list.items():
            if path.isdigit():
                print(f"----> Custom executable ID provided for [{name}]: {path}")
                id_found = True
            else:
                if not Path(path).is_file():
                    print(f"----> [ERROR] The provided path for [{name}] is not a valid file: {path}")
                    print(f"----> [ERROR] Please check the start-benchmark-configuration.json file")
                    raise
                print(f"----> Custom executable path provided for [{name}]: {path}")
            executables[name] = path

        # Verify GitHub CLI login status if any ID is found
        if id_found:
            try:
                print("--> Verifying GitHub CLI login status...")
                subprocess.run("gh auth status", shell=True, text=True, capture_output=True, check=True)
                print(f"----> GitHub CLI is logged in!")
            except Exception as e:
                print(f"----> [ERROR] GitHub CLI is not logged in. Please run 'gh auth login' to log in.")
                print(f"----> [ERROR] {e}")
                raise
            
        # Download executables from GitHub Actions using 'gh' CLI
        ARTIFACT_FILE_NAME = "MPM-Geomechanics-benchmark"
        artifact_name = ""
        extension = ""
        if sys.platform == "win32" or sys.platform == "cygwin":
            artifact_name = "MPM-Geomechanics-benchmark-windows"
            extension = ".exe"

        if sys.platform == "linux":
            artifact_name = "MPM-Geomechanics-benchmark-linux"

        if artifact_name == "":
            print(f"----> [ERROR] Unsupported platform: {sys.platform}")
            raise

        for name, path in executables_list.items():
            if path.isdigit():
                print(f"\n----> Checking GitHub for run ID [{path}] for executable [{name}]...")
                try:
                    print(f"------> Checking if the run ID [{path}] exists in GitHub Actions (command: gh run view {path})...")
                    subprocess.run(f"gh run view {path}", shell=True, text=True, capture_output=True, check=True)
                    print(f"------> ID [{path}] exists")
                except Exception as e:
                    print(f"------> [ERROR] An error occurred while checking the run ID [{path}]")
                    print(f"------> [ERROR] Please check if the run ID [{path}] exists in GitHub Actions")
                    print(f"------> [ERROR] {e}")
                    raise

                try:
                    print(f"------> Creating folder for [{name}] in {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}...")
                    Path(f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}").mkdir(parents=True, exist_ok=True)
                    print(f"------> Downloading the executable for [{name}] to {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER} (command: gh run download {path} --name {artifact_name} -D {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name})...")
                    subprocess.run(f"gh run download {path} --name {artifact_name} -D {BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}", shell=True, text=True, capture_output=True, check=True)
                    print(f"------> Download completed for [{name}]")

                    # Set the executable path (example: benchmark/artifact/current/MPM-Geomechanics-benchmark <-- or MPM-Geomechanics-benchmark.exe)
                    executables[name] = f"{BENCHMARK_FOLDER}/{ARTIFACT_FOLDER}/{name}/{ARTIFACT_FILE_NAME}" + extension
                except Exception as e:
                    print(f"----> [ERROR] An error occurred while downloading executable for [{name}]")
                    print(f"----> [ERROR] Please check if the run ID [{path}] exists in GitHub Actions")
                    print(f"----> [ERROR] {e}")
                    raise
                

    if "parameters" in json_configuration:
        print("\n--> Custom parameters found in configuration file")
        executables_parameters = json_configuration["parameters"]
        materials_point = executables_parameters["particles"] if "particles" in executables_parameters else materials_point
        threads = executables_parameters["threads"] if "threads" in executables_parameters else threads
        print(f"----> Materials points: {materials_point}")
        print(f"----> Threads: {threads}")

    # Verify if executables exist
    if executables.values():
        print("\n--> Verifying executables paths")
        for name, path in executables.items():
            if not Path(path).is_file():
                print(f"----> [ERROR] The provided path for [{name}] is not a valid file: {path}")
                print(f"----> [ERROR] Please check the start-benchmark-configuration.json file")
                raise
        print ("----> All executables paths are valid")
        
    if executables.values():
        print("\n--> Executables to be used:")
        for name, path in executables.items():
            print(f"----> [{name}]: {path}")

    print(f"--> All parameters read successfully")

    # Save cached configuration
    print(f"\n--> Saving cached configuration to {CACHE_FILE_NAME}")
    with open(f"{BENCHMARK_FOLDER}/{CACHE_FILE_NAME}", "w") as f:
        cache = {"executables": executables, "parameters": {"particles": materials_point, "threads": threads}}
        json.dump(cache, f, indent=4)
        print(f"----> Cached configuration saved to {BENCHMARK_FOLDER}/{CACHE_FILE_NAME}")

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
        # Read command line flags
        read_flags()
        
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
    except Exception as e:
        print(f"--> [ERROR] An error occurred during the benchmarking process")
        print(f"--> [ERROR] {e}")
        return -1

# Start benchmarking process
main()