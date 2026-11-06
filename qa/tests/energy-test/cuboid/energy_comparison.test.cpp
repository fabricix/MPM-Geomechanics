#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <MPM.h>
#include <filesystem>

double sum_energy(const std::string &filename)
{

  std::ifstream file(filename);
  std::string line;
  double sum = 0.0;

  if (!file.is_open())
  {
    std::cerr << "Error opening file: " << filename << std::endl;
    return -1.0;
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
        sum += std::stod(second);
      }
      catch (...)
      {
        std::cerr << "Error converting value to double in file: " << filename
                  << " at line: " << line << std::endl;
        return -1.0;
      }
    }
  }

  return sum;
}

double analyticalEnergy(const std::string &time_energy_file)
{
  // Constants from the analytical energy calculation
  const double g = 9.81;    // gravity (m/s²)
  const double m = 64000.0; // mass (kg)
  const double dt = 0.001;  // time step offset (s)

  std::ifstream file(time_energy_file);
  std::string line;
  double sum = 0.0;

  if (!file.is_open())
  {
    std::cerr << "Error opening file: " << time_energy_file << std::endl;
    return -1.0;
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
        std::cout << "Analytical energy at time " << t << " : " << rounded_energy_str << std::endl;

        sum += rounded_energy;
      }
      catch (...)
      {
        std::cerr << "Error converting value to double in file: " << time_energy_file
          << " at line: " << line << std::endl;
        return -1.0;
      }
    }
  }

  return sum;
}

void executeSimulation()
{
  std::filesystem::path test_dir = std::filesystem::current_path() / ".." / ".." / "qa" / "tests" / "energy-test";
  std::filesystem::current_path(test_dir);
  std::filesystem::path exe_path;
  std::string script;

  try
  {
    exe_path = std::filesystem::path("..") / ".." / ".." / "build" / "CMake" / "MPM-Geomechanics";
#ifdef _WIN32
    exe_path += ".exe";
#endif

    script = exe_path.string() + " cuboid.json";
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

TEST(ENERGY_COMPARISON, ENERGY_SUM)
{
  executeSimulation();

  double main_energy = analyticalEnergy("time-energy.csv");
  double test_energy = sum_energy("time-energy.csv");

  if (main_energy == -1.0 || test_energy == -1.0)
  {
    FAIL() << "Could not calculate energy from one or both files.";
  }

  EXPECT_EQ(main_energy, test_energy);
}
