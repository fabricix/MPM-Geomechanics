#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream> 
#include "Json/json.hpp"
#include "performance/configuration.performance.h"

using namespace std;
using json = nlohmann::json;

namespace Configuration
{

    json configuration;
    string jsonFileName = "test-configuration.json";

    int numParticles = 0;
    int numThreads = 0;
    Vector3d particleSize = Vector3d::Zero();
    double particleMass = 0.0;
    Vector3d cellDimension = Vector3d::Zero();
    Vector3i numCells = Vector3i::Zero();
    int randomSeed = 0;

    int getNumParticles() { return numParticles; }
    int getNumThreads() { return numThreads; }
    Vector3d getParticleSize() { return particleSize; }
    double getParticleMass() { return particleMass; }
    Vector3d getCellDimension() { return cellDimension; }
    Vector3i getNumCells() { return numCells; }
    int getRandomSeed() { return randomSeed; }

    void setNumParticles(int n) { numParticles = n; }
    void setNumThreads(int n) { numThreads = n; }
    void setParticleSize(const Vector3d& size) { particleSize = size; }
    void setParticleMass(double mass) { particleMass = mass; }
    void setCellDimension(const Vector3d& dimension) { cellDimension = dimension; }
    void setNumCells(const Vector3i& cells) { numCells = cells; }
    void setRandomSeed(int seed) { randomSeed = seed; }

    void openConfiguration()
    {
        cout << "> Open configuration file..." << endl;

        vector<string> argv = ::testing::internal::GetArgvs();

        if (argv.size() > 1 && argv[1] == "--config")
        {
            if (!argv[2].empty())
            {
                try
                {
                    jsonFileName = argv[2];
                    cout << "--> Specifying configuration file: " << jsonFileName << endl;
                }
                catch (...)
                {
                    throw runtime_error("Failed to set configuration file: " + jsonFileName);
                }
            }
            else
            {
                throw runtime_error("No configuration file specified");
            }
        }
        else if (argv.size() > 1)
        {
            throw runtime_error("Invalid argument: " + argv[1]);
        }

        ifstream file(jsonFileName);

        if (!file.is_open()) {
            cerr << "--> [ERROR] Error opening file: " << jsonFileName << endl;
            throw runtime_error("Configuration file not found");
        }
        else
        {
            cout << "--> File opened successfully: " << jsonFileName << endl;
        }

        file >> configuration;
    }

    void readConfiguration()
    {
        cout << "> Read content from configuration file..." << endl;
        string step;
        string expected;

        try
        {
            step = "setting num_particles";
            numParticles = int(configuration["num_particles"]);
            setNumParticles(numParticles);

            step = "setting num_threads";
            numThreads = int(configuration["num_threads"]);
            setNumThreads(numThreads);

            step = "setting particle_size";
            particleSize = Vector3d(int(configuration["particle_size"][0]), int(configuration["particle_size"][1]), int(configuration["particle_size"][2]));
            setParticleSize(particleSize);

            step = "setting particle_mass";
            particleMass = int(configuration["particle_mass"]);
            setParticleMass(particleMass);

            step = "setting cell_dimension";
            cellDimension = Vector3d(int(configuration["cell_dimension"][0]), int(configuration["cell_dimension"][1]), int(configuration["cell_dimension"][2]));
            setCellDimension(cellDimension);

            step = "setting num_cells";
            numCells = Vector3i(int(configuration["num_cells"][0]), int(configuration["num_cells"][1]), int(configuration["num_cells"][2]));
            setNumCells(numCells);

            step = "setting random_seed";
            randomSeed = int(configuration["random_seed"]);
            setRandomSeed(randomSeed);

            cout << "--> Configuration read successfully." << endl;
            cout << "--> Displaying configuration values:" << endl;
            cout << "----> numParticles: " << getNumParticles() << endl;
            cout << "----> numThreads: " << getNumThreads() << endl;
            cout << "----> particleSize: [" << getParticleSize()[0] << ", " << getParticleSize()[1] << ", " << getParticleSize()[2] << "]" << endl;
            cout << "----> particleMass: " << getParticleMass() << endl;
            cout << "----> cellDimension: [" << getCellDimension()[0] << ", " << getCellDimension()[1] << ", " << getCellDimension()[2] << "]" << endl;
            cout << "----> numCells: [" << getNumCells()[0] << ", " << getNumCells()[1] << ", " << getNumCells()[2] << "]" << endl;
            cout << "----> randomSeed: " << getRandomSeed() << endl;
        }
        catch (const std::exception& e)
        {
            cerr << "--> [ERROR] Failed at step: " << step << ". Please check the json file." << endl;
            cerr << "--> [ERROR] Error reading configuration:" << e.what() << endl;
            throw (0);
        }
    }


    int setup()
    {
        int status = 0;

        cout << "------------------------" << endl;
        cout << "SETUP CONFIGURATION" << endl;
        cout << "\n";
        try
        {
            openConfiguration();
            cout << "\n";

            readConfiguration();
            cout << "\n";

            status = 0;
        }
        catch (const std::exception& e)
        {
            cerr << "[ERROR] Error during configuration setup: " << e.what() << endl;
            status = -1;

        }

        cout << "Setup complete with status: " << status << endl;
        cout << "------------------------" << endl;
        cout << "\n";

        return status;
    }
};