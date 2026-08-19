# Specify the minimum required CMake version for this project
cmake_minimum_required(VERSION 3.10)

# Project name
project(MPM-GEOMECHANICS-PROJECT)

# OpenMP Package
find_package(OpenMP REQUIRED)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directories for the compiled binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Flags
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O0 -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -O3 -march=native -flto=auto -ffast-math -funroll-loops -DNDEBUG -fopenmp -std=c++17")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -march=native -ffast-math -funroll-loops -DNDEBUG -fopenmp -std=c++17")

# Set parallel level
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

# Add include directory
include_directories(../../inc)
include_directories(../../inc/Particle)
include_directories(../../inc/Shape)
include_directories(../../inc/Body)
include_directories(../../inc/Materials)
include_directories(../../inc/Mesh)

# Add executables for the MPM-GEOMECHANICS project
file(GLOB EXECUTABLES "../../src/*.cpp")

add_executable(MPM-Geomechanics ${EXECUTABLES})
target_compile_definitions(MPM-Geomechanics PRIVATE USE_PARALLEL_COMPUTATION)