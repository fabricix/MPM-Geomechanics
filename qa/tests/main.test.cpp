// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <string>

using namespace std;

bool verifyExecutableExists()
{
  std::filesystem::path exe_path;

  // Construct the path to the executable
  exe_path = std::filesystem::path("..") / ".." / "build" / "CMake" / "MPM-Geomechanics";
#ifdef _WIN32
  exe_path += ".exe";
#endif

  // Verify if the binary exists
  if (!std::filesystem::exists(exe_path))
  {
    std::cerr << "\n"
              << std::string(80, '=') << std::endl;
    std::cerr << "ERROR: MPM-Geomechanics executable not found!" << std::endl;
    std::cerr << std::string(80, '=') << std::endl;
    std::cerr << "\nExpected path: " << exe_path.string() << std::endl;
    std::cerr << "\nThe simulation executable must be built before running any tests." << std::endl;
    std::cerr << "\nPlease build the project first by running:" << std::endl;
#ifdef _WIN32
    std::cerr << "\n  Use the provided batch script:" << std::endl;
    std::cerr << "  .\\build\\CMake\\cmake-build.bash" << std::endl;
    std::cerr << "\n  Or build manually:" << std::endl;
    std::cerr << "  cd build\\CMake" << std::endl;
    std::cerr << "  cmake -B . -S ../.." << std::endl;
    std::cerr << "  cmake --build . --config Release" << std::endl;
#else
    std::cerr << "\n  Use the provided build script:" << std::endl;
    std::cerr << "  ./build/CMake/cmake-build.bash" << std::endl;
    std::cerr << "\n  Or build manually:" << std::endl;
    std::cerr << "  cd build/CMake" << std::endl;
    std::cerr << "  cmake -B . -S ../.." << std::endl;
    std::cerr << "  cmake --build . --config Release" << std::endl;
#endif
    std::cerr << "\nFor more information, see the README.md in the project root." << std::endl;
    std::cerr << std::string(80, '=') << "\n"
              << std::endl;
    return false;
  }

  std::cout << "\n✓ Executable verified: " << exe_path.string() << std::endl;
  std::cout << "✓ All prerequisite checks passed. Starting tests...\n"
            << std::endl;
  return true;
}

int main(int argc, char **argv)
{
  // Verify the executable exists before running any tests
  if (!verifyExecutableExists())
  {
    std::cerr << "ERROR: Cannot proceed without the MPM-Geomechanics executable." << std::endl;
    std::cerr << "Please build the project and try again." << std::endl;
    return 1;
  }

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}