#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

TEST(ENERGY_COMPARISON, ENERGY_SUM)
{
  double main_energy = sum_energy("test-files/time-eneasdrgy-main.csv");
  double test_energy = sum_energy("test-files/time-energasdasdy-test03.csv");

  if (main_energy == -1.0 || test_energy == -1.0) {
    FAIL() << "Could not calculate energy from one or both files.";
  }

  EXPECT_EQ(main_energy, test_energy);
}

