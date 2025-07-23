#!/bin/bash

# Run cmake to generate build files
cmake -G "Unix Makefiles" -B build
if [ $? -ne 0 ]; then
  echo "Error: CMake generation failed."
  exit 1
fi

# Build the project
cmake --build build
if [ $? -ne 0 ]; then
  echo "Error: Project build failed."
  exit 1
fi

echo "Project successfully built."
