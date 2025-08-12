# Specify the minimum required CMake version for this project
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(Testing)

# Enable OpenMP
find_package(OpenMP REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Add the subdirectory where GoogleTest is located (embedded in "external/googletest")
# This will allow CMake to compile it along with the project
add_subdirectory(../../external/googletest ${CMAKE_BINARY_DIR}/googletest)

# Add the GoogleTest include directory to be able to use #include <gtest/gtest.h>
include_directories(../../external/googletest/googletest/include)
include_directories(../../inc)
include_directories(../../inc/Particle)
include_directories(../../inc/Shape)
include_directories(../../inc/Body)
include_directories(../../inc/Materials)
include_directories(../../inc/Mesh)

# ====================================
# Add executables for numerical tests
# ====================================

# Remove main.cpp file from the list of sources to avoid multiple definitions of the main function
file(GLOB_RECURSE TEST_SOURCES "../../tests/*.test.cpp" "main.test.cpp")
file(GLOB_RECURSE EXECUTABLES "../../src/*.cpp")
list(REMOVE_ITEM EXECUTABLES "${CMAKE_CURRENT_SOURCE_DIR}/../../src/Main.cpp")
add_executable(MPM-Geomechanics-tests ${TEST_SOURCES} ${EXECUTABLES})

# Define USE_PARALLEL_INTERPOLATION to enable parallel interpolation in the test executable
target_compile_definitions(MPM-Geomechanics-tests PRIVATE USE_PARALLEL_COMPUTATION)

# Link the executable with the GoogleTest libraries (gtest and gtest_main)
# gtest: GoogleTest's main library
# gtest_main: Provides a main() function to run the tests
target_link_libraries(MPM-Geomechanics-tests gtest gtest_main)

# ==============================================
# Add benchmark executable for performance tests
# ==============================================

# Gather all performance test files (*.performance.cpp)
file(GLOB_RECURSE PERFORMANCE_SOURCES "../../tests/*.performance.cpp")

# Reuse the simulator sources (same as for MPM-Geomechanics-tests)
add_executable(MPM-Geomechanics-benchmark ${PERFORMANCE_SOURCES} ${EXECUTABLES})

# Define USE_PARALLEL_INTERPOLATION to enable parallel interpolation in the benchmark executable
#target_compile_definitions(MPM-Geomechanics-benchmark PRIVATE USE_PARALLEL_COMPUTATION)
target_compile_definitions(MPM-Geomechanics-benchmark PRIVATE USE_PARALLEL_INTERPOLATION)

# Link Google Test and OpenMP
target_link_libraries(MPM-Geomechanics-benchmark gtest gtest_main OpenMP::OpenMP_CXX)
