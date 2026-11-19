// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <string>

using namespace std;

bool verifyExecutableExists()
{
  const std::filesystem::path exe_path{MPM_EXECUTABLE_PATH};

  // Verify if the binary exists
  if (!std::filesystem::is_regular_file(exe_path))
  {
    std::cerr << "\n"
              << std::string(80, '=') << std::endl;
    std::cerr << "ERROR: MPM-Geomechanics executable not found!" << std::endl;
    std::cerr << std::string(80, '=') << std::endl;
    std::cerr << "\nExpected path: " << exe_path.string() << std::endl;
    std::cout << "Build the simulation executable now? [y/N]: " << std::flush;
    std::string answer;
    if (!std::getline(std::cin, answer) ||
        (answer != "y" && answer != "Y" && answer != "yes" &&
         answer != "Yes" && answer != "s" && answer != "S" && answer != "si"))
    {
      std::cerr << "Build cancelled." << std::endl;
      return false;
    }

    const auto script_dir = std::filesystem::path{MPM_PROJECT_ROOT} / "build" / "CMake";
    if (!std::filesystem::is_regular_file(script_dir / "cmake-build.bash"))
    {
      std::cerr << "ERROR: Build script not found in " << script_dir.string() << std::endl;
      return false;
    }

    // Use a fixed command in the script directory, then restore the test directory.
    // The script also anchors its source/build paths to its own location.
    const auto original_dir = std::filesystem::current_path();
    std::filesystem::current_path(script_dir);
    const int build_result = std::system("bash ./cmake-build.bash");
    std::filesystem::current_path(original_dir);

    if (build_result != 0)
    {
      std::cerr << "ERROR: Simulation build failed. Check the build output above." << std::endl;
      return false;
    }
    if (!std::filesystem::is_regular_file(exe_path))
    {
      std::cerr << "ERROR: Build finished but the executable is still missing: "
                << exe_path.string() << std::endl;
      return false;
    }
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