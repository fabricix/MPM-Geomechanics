#!/bin/bash

# Test executable names for Linux and Windows
TEST_EXECUTABLE_LINUX="MPM-Geomechanics-tests"
TEST_EXECUTABLE_WINDOWS="MPM-Geomechanics-tests.exe"

# Paths to test executables
BIN_PATH_LINUX="./$TEST_EXECUTABLE_LINUX"
BIN_PATH_WINDOWS="./$TEST_EXECUTABLE_WINDOWS"

# Remove Linux test executable if it exists
if [ -f "$BIN_PATH_LINUX" ]; then
  echo "Removing test executable (Linux): $BIN_PATH_LINUX"
  rm "$BIN_PATH_LINUX"
else
  echo "No Linux test executable found at $BIN_PATH_LINUX"
fi

# Remove Windows test executable if it exists
if [ -f "$BIN_PATH_WINDOWS" ]; then
  echo "Removing test executable (Windows): $BIN_PATH_WINDOWS"
  rm "$BIN_PATH_WINDOWS"
else
  echo "No Windows test executable found at $BIN_PATH_WINDOWS"
fi

# Remove build directory
if [ -d "build" ]; then
  echo "Removing build directory..."
  rm -rf build
else
  echo "No build directory to remove."
fi

echo "Clean-up complete."
