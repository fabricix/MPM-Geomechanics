#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <MPM.h>
#include <filesystem>

double sum_energy(const std::string& filename)
{

  std::ifstream file(filename);
  std::string line;
  double sum = 0.0;

  if (!file.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return -1.0;
  }

  // Saltar encabezado
  std::getline(file, line);

  // Leer cada línea
  while (std::getline(file, line)) {
    std::istringstream ss(line);
    std::string first, second;

    if (std::getline(ss, first, ',') && std::getline(ss, second, ',')) {
      try {
        sum += std::stod(second);
      }
      catch (...) {
        std::cerr << "Error converting value to double in file: " << filename
                  << " at line: " << line << std::endl;
        return -1.0;
      }
    }
  }

  return sum;
}

void executeSimulation()
{

  std::filesystem::path test_dir = std::filesystem::current_path() / ".." / ".." / "tests" / "numerical-verification" / "energy-test";
  std::filesystem::current_path(test_dir);

  try
  {
    int response = system("..\\..\\..\\build\\CMake\\MPM-Geomechanics.exe cuboid.json");

    if (response == 0)
    {
      std::cout << "Simulation executed successfully." << std::endl;
    }
    else
    {
      std::cerr << "Simulation failed with response code: " << response << std::endl;
      std::cerr << "Please check if simulation program exists and try again." << std::endl;
      FAIL() << "Simulation execution failed.";
      return;
    }

    string filename = "time-energy.csv";

    std::ifstream file(filename);
    std::string line;
    double sum = 0.0;

    if (!file.is_open()) {
      std::cerr << "Error opening file: " << filename << std::endl;
    }
    else
    {
      std::cout << "File opened successfully: " << filename << std::endl;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception during simulation execution: " << e.what() << std::endl;
    FAIL() << "Simulation execution failed due to exception.";
  }
}

TEST(ENERGY_COMPARISON, ENERGY_SUM)
{

  executeSimulation();

  double main_energy = sum_energy("analytical_energy.csv");
  double test_energy = sum_energy("time-energy.csv");

  if (main_energy == -1.0 || test_energy == -1.0) {
    FAIL() << "Could not calculate energy from one or both files.";
  }

  EXPECT_EQ(main_energy, test_energy);
}

