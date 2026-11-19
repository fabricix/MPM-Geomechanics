#!/bin/bash

# Keep generated files here regardless of the caller's working directory.
source_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)" || exit 1
build_dir="${source_dir}/build"

# Run cmake to generate build files
cmake -G "Unix Makefiles" -S "$source_dir" -B "$build_dir"
if [ $? -ne 0 ]; then
  echo "Error: CMake generation failed."
  exit 1
fi

# Build the project
cmake --build "$build_dir"
if [ $? -ne 0 ]; then
  echo "Error: Project build failed."
  exit 1
fi

echo "Project successfully built."
