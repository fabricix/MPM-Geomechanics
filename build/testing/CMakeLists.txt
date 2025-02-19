# Specify the minimum required CMake version for this project
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(Testing)

# Add the subdirectory where GoogleTest is located (embedded in "external/googletest")
# This will allow CMake to compile it along with the project
add_subdirectory(../../external/googletest ${CMAKE_BINARY_DIR}/googletest)

# Add the GoogleTest include directory to be able to use #include <gtest/gtest.h>
include_directories(../../external/googletest/googletest/include)
include_directories(../../inc)

# Search for all source files ending in ".test.cpp" in the "tests" directory
file(GLOB_RECURSE TEST_SOURCES "../../tests/*.test.cpp")

# Define an executable named "MPM-Test" and add all the test source files
add_executable(MPM-Test main.test.cpp ${TEST_SOURCES})

# Link the executable with the GoogleTest libraries (gtest and gtest_main)
# gtest: GoogleTest's main library
# gtest_main: Provides a main() function to run the tests
target_link_libraries(MPM-Test gtest gtest_main)