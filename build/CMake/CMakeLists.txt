# Specify the minimum required CMake version for this project
cmake_minimum_required(VERSION 3.10)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)

# Project name
project(MPM-GEOMECHANICS)

# Add include directory
include_directories(../../inc)
include_directories(../../inc/Particle)
include_directories(../../inc/Shape)
include_directories(../../inc/Body)
include_directories(../../inc/Materials)
include_directories(../../inc/Mesh)

# Add executables for the MPM-GEOMECHANICS project
file(GLOB_RECURSE EXECUTABLES "../../src/*.cpp")
add_executable(MPM-GEOMECHANICS ${EXECUTABLES})
