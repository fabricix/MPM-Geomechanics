#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <MPM.h>
#include <filesystem>

std::vector<double> sum_energy(const std::string &filename)
{

  std::ifstream file(filename);
  std::string line;
  std::vector<double> energies;

  if (!file.is_open())
  {
    std::cerr << "Error opening file: " << filename << std::endl;
    return std::vector<double>();
  }

  // Saltar encabezado
  std::getline(file, line);

  // Leer cada línea
  while (std::getline(file, line))
  {
    std::istringstream ss(line);
    std::string first, second;

    if (std::getline(ss, first, ',') && std::getline(ss, second, ','))
    {
      try
      {
        energies.push_back(std::stod(second));
      }
      catch (...)
      {
        std::cerr << "Error converting value to double in file: " << filename
                  << " at line: " << line << std::endl;
        return std::vector<double>();
      }
    }
  }

  return energies;
}

std::vector<double> analyticalEnergy(const std::string &time_energy_file)
{
  // Constants from the analytical energy calculation
  const double g = 9.81;    // gravity (m/s²)
  const double m = 64000.0; // mass (kg)
  const double dt = 0.001;  // time step offset (s)

  std::ifstream file(time_energy_file);
  std::string line;
  std::vector<double> energies;

  if (!file.is_open())
  {
    std::cerr << "Error opening file: " << time_energy_file << std::endl;
    return std::vector<double>();
  }

  // Skip header
  std::getline(file, line);

  // Read each time value and calculate analytical energy
  while (std::getline(file, line))
  {
    std::istringstream ss(line);
    std::string time_str, energy_str;

    if (std::getline(ss, time_str, ',') && std::getline(ss, energy_str, ','))
    {
      try
      {
        double t = std::stod(time_str);
        double energy;

        // Calculate analytical energy: E(t) = 0.5 * m * g² * (t - dt/2)²
        if (t == 0.0)
        {
          energy = 0.0;
        }
        else
        {
          double g_squared = g * g;
          double time_offset = t - (dt / 2.0);
          energy = 0.5 * m * g_squared * (time_offset * time_offset);
        }

        std::ostringstream oss;
        oss << std::scientific << std::setprecision(5) << energy;
        std::string rounded_energy_str = oss.str();
        double rounded_energy = std::stod(rounded_energy_str);

        energies.push_back(rounded_energy);
      }
      catch (...)
      {
        std::cerr << "Error converting value to double in file: " << time_energy_file
                  << " at line: " << line << std::endl;
        return std::vector<double>();
      }
    }
  }

  return energies;
}

void executeSimulation()
{
  try
  {
    std::cout << "Verifying simulation environment..." << std::endl;

    const std::filesystem::path test_dir{MPM_TEST_DIR};
    const std::filesystem::path exe_path{MPM_EXECUTABLE_PATH};

    if (!std::filesystem::is_directory(test_dir))
      throw std::runtime_error("No test directory found");

    if (!std::filesystem::is_regular_file(exe_path))
      throw std::runtime_error("No executable found");

    std::filesystem::current_path(test_dir);

    std::cout << "Starting Simulation: " << std::endl;

    std::string script = exe_path.string() + " cuboid.json";
    std::cout << "Executing script: " << script << std::endl;
    int response = system(script.c_str());

    if (response == 0)
    {
      std::cout << "Simulation executed successfully." << std::endl;
    }
    else
    {
      std::cerr << "Simulation failed with response code: " << response << std::endl;
      std::cerr << "Please check the simulation output above for details." << std::endl;
      FAIL() << "Could not execute simulation.";
      return;
    }

    std::string filename = "time-energy.csv";
    std::ifstream file(filename);

    if (!file.is_open())
    {
      std::cerr << "Error opening file: " << filename << std::endl;
      FAIL() << "Could not open time-energy file.";
    }
    else
    {
      std::cout << "File opened successfully: " << filename << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Exception during simulation execution: " << e.what() << std::endl;
    FAIL() << "Simulation execution failed due to exception.";
  }
}

TEST(ENERGY_COMPARISON, CUBOID_ENERGY)
{
  executeSimulation();

  std::vector<double> main_energies = analyticalEnergy("time-energy.csv");
  std::vector<double> test_energies = sum_energy("time-energy.csv");

  if (main_energies.empty() || test_energies.empty())
  {
    FAIL() << "Could not calculate energy from one or both files.";
  }

  ASSERT_EQ(main_energies.size(), test_energies.size()) << "Energy vectors have different sizes.";

  for (size_t i = 0; i < main_energies.size(); ++i)
  {
    EXPECT_EQ(main_energies[i], test_energies[i]);
  }
}
