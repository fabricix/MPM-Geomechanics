#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream> 
#include "Json/json.hpp"
#include "configuration.performance.h"

using namespace std;
#include <iomanip>

using json = nlohmann::json;

namespace Spaces
{
    string getTextFormatted(string str01, float str02, int spacing)
    {
        string spaces = "";
        int separation = spacing - str01.length();
        if (separation < 1) separation = 0;
        spaces += string(separation, ' ');
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << str02;
        string value_to_2 = stream.str();
        return str01 + spaces + value_to_2;
    }
}

namespace Configuration
{

    json configuration;
    json globalConfiguration;
    string jsonFileName = "test-configuration.json";

    // Global variables
    int numParticles = 0;
    int numThreads = 0;
    Vector3d particleSize = Vector3d::Zero();
    double particleMass = 0.0;
    Vector3d cellDimension = Vector3d::Zero();
    Vector3i numCells = Vector3i::Zero();
    int randomSeed = 0;

    // Update - UpdatePerformance_ParticleDensity_nParticles - Variables
    double initialDensity = 2000;
    double traceStrain = 0.05;
    double expectedDensity = 2000 / (1.0 + 0.05);

    // Update - ParticleStress_nParticles - Variables
    double E = 1000.0;
    double nu = 0.3;
    double epsilon0 = 0.01;

    // Test flags
    bool interpolationTest = true;
    bool updateTest = true;

    // Global variables - Functions
    int getNumParticles() { return numParticles; }
    void setNumParticles(int n) { numParticles = n; }

    int getNumThreads() { return numThreads; }
    void setNumThreads(int n) { numThreads = n; }

    Vector3d getParticleSize() { return particleSize; }
    void setParticleSize(const Vector3d& size) { particleSize = size; }

    double getParticleMass() { return particleMass; }
    void setParticleMass(double mass) { particleMass = mass; }

    Vector3d getCellDimension() { return cellDimension; }
    void setCellDimension(const Vector3d& dimension) { cellDimension = dimension; }

    Vector3i getNumCells() { return numCells; }
    void setNumCells(const Vector3i& cells) { numCells = cells; }

    int getRandomSeed() { return randomSeed; }
    void setRandomSeed(int seed) { randomSeed = seed; }

    // Update - UpdatePerformance_ParticleDensity_nParticles - Functions
    double getInitialDensity() { return initialDensity; }
    void setInitialDensity(double density) { initialDensity = density; }

    double getTraceStrain() { return traceStrain; }
    void setTraceStrain(double strain) { traceStrain = strain; }

    double getExpectedDensity() { return expectedDensity; }
    void setExpectedDensity(double density) { expectedDensity = density; }
    void setDefaultExpectedDensity() { expectedDensity = initialDensity / (1.0 + traceStrain); }

    // Update - ParticleStress_nParticles - Functions
    double getE() { return E; }
    void setE(double value) { E = value; }

    double getNu() { return nu; }
    void setNu(double value) { nu = value; }

    double getEpsilon0() { return epsilon0; }
    void setEpsilon0(double value) { epsilon0 = value; }

    // Test flags - Functions
    bool getInterpolationTest() { return interpolationTest; }
    void setInterpolationTest(bool value) { interpolationTest = value; }

    bool getUpdateTest() { return updateTest; }
    void setUpdateTest(bool value) { updateTest = value; }

    // dictionary
    unordered_map<string, function<float()>> configGetters = {
        {"initialDensity",  getInitialDensity},
        {"traceStrain",  getTraceStrain},
        {"expectedDensity",  getExpectedDensity},
        {"E",  getE},
        {"nu",  getNu},
        {"epsilon0",  getEpsilon0}
    };

    json getVar(string key)
    {
        json value = 0.0;
        try
        {
            value = configuration[key];
            return value;
        }
        catch (const std::exception& e)
        {
            try
            {
                value = configGetters[key]();
                string message = "--> [WARNING] Using default value for: " + key + ": ";
                cout << Spaces::getTextFormatted(message, value, 55) << endl;
                return value;
            }
            catch (const std::exception& e)
            {
                cerr << "--> [ERROR] Failed to get default configuration variable: " << e.what() << endl;
                throw;
            }
        }
    }

    float getUpdateVar(string key)
    {
        return getVar("UpdatePerformance_ParticleDensity_nParticles")[key];
    }

    void openConfiguration()
    {
        cout << "> Open configuration file..." << endl;

        vector<string> argv = ::testing::internal::GetArgvs();

        if (argv.size() > 1 && !argv[1].empty())
        {
            try
            {
                jsonFileName = argv[1];
                cout << "--> Specifying configuration file: " << jsonFileName << endl;
            }
            catch (...)
            {
                cerr << "--> [ERROR] Error opening file: " << jsonFileName << endl;
                throw runtime_error("Failed to set configuration file: " + jsonFileName);
            }
        }
        else
        {
            cout << "--> Using default configuration file: " << jsonFileName << endl;
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

        globalConfiguration = configuration["global"];
    }

    void readGlobalConfiguration()
    {
        cout << "> Read global configuration..." << endl;
        string step;
        string expected;

        try
        {
            // Global variables
            step = "setting num_particles";
            numParticles = int(globalConfiguration["num_particles"]);
            setNumParticles(numParticles);

            step = "setting num_threads";
            numThreads = int(globalConfiguration["num_threads"]);
            setNumThreads(numThreads);

            step = "setting particle_size";
            particleSize = Vector3d(int(globalConfiguration["particle_size"][0]), int(globalConfiguration["particle_size"][1]), int(globalConfiguration["particle_size"][2]));
            setParticleSize(particleSize);

            step = "setting particle_mass";
            particleMass = int(globalConfiguration["particle_mass"]);
            setParticleMass(particleMass);

            step = "setting cell_dimension";
            cellDimension = Vector3d(int(globalConfiguration["cell_dimension"][0]), int(globalConfiguration["cell_dimension"][1]), int(globalConfiguration["cell_dimension"][2]));
            setCellDimension(cellDimension);

            step = "setting num_cells";
            numCells = Vector3i(int(globalConfiguration["num_cells"][0]), int(globalConfiguration["num_cells"][1]), int(globalConfiguration["num_cells"][2]));
            setNumCells(numCells);

            step = "setting random_seed";
            randomSeed = int(globalConfiguration["random_seed"]);
            setRandomSeed(randomSeed);

            step = "setting interpolation_test";
            if (globalConfiguration.contains("interpolation_test"))
            {
                interpolationTest = globalConfiguration["interpolation_test"];
            }
            setInterpolationTest(interpolationTest);

            step = "setting update_test";
            if (globalConfiguration.contains("update_test"))
            {
                updateTest = globalConfiguration["update_test"];
            }
            setUpdateTest(updateTest);

            cout << "--> Global Configuration read successfully." << endl;
            cout << "--> Displaying configuration values:" << endl;
            cout << "----> numParticles:        " << getNumParticles() << endl;
            cout << "----> numThreads:          " << getNumThreads() << endl;
            cout << "----> particleSize:        [" << getParticleSize()[0] << ", " << getParticleSize()[1] << ", " << getParticleSize()[2] << "]" << endl;
            cout << "----> particleMass:        " << getParticleMass() << endl;
            cout << "----> cellDimension:       [" << getCellDimension()[0] << ", " << getCellDimension()[1] << ", " << getCellDimension()[2] << "]" << endl;
            cout << "----> numCells:            [" << getNumCells()[0] << ", " << getNumCells()[1] << ", " << getNumCells()[2] << "]" << endl;
            cout << "----> randomSeed:          " << getRandomSeed() << endl;
            cout << "--> Test Flags" << endl;
            cout << "----> interpolationTest:   " << (getInterpolationTest() ? "true" : "false") << endl;
            cout << "----> updateTest:          " << (getUpdateTest() ? "true" : "false") << endl;
        }
        catch (const std::exception& e)
        {
            cerr << "--> [ERROR] Failed at step: " << step << ". Please check the json file." << endl;
            cerr << "--> [ERROR] Error reading configuration:" << e.what() << endl;
            throw (0);
        }
    }

    void readSpecificConfiguration()
    {
        cout << "> Read specific configuration..." << endl;
        string step;

        try
        {
            // --> Update - UpdatePerformance_ParticleDensity_nParticles
            step = "setting initialDensity";
            initialDensity = float(configuration["UpdatePerformance_ParticleDensity_nParticles"]["initialDensity"]);
            setInitialDensity(initialDensity);

            step = "setting traceStrain";
            traceStrain = float(configuration["UpdatePerformance_ParticleDensity_nParticles"]["traceStrain"]);
            setTraceStrain(traceStrain);

            // The expected density is calculated based on the initial density and trace strain
            step = "setting expectedDensity";
            setDefaultExpectedDensity();

            // --> Update - ParticleStress_nParticles
            step = "setting E";
            E = float(configuration["ParticleStress_nParticles"]["E"]);
            setE(E);

            step = "setting nu";
            nu = float(configuration["ParticleStress_nParticles"]["nu"]);
            setNu(nu);

            step = "setting epsilon0";
            epsilon0 = float(configuration["ParticleStress_nParticles"]["epsilon0"]);
            setEpsilon0(epsilon0);

            cout << "--> Specific Configuration read successfully." << endl;
            cout << "--> Displaying configuration values:" << endl;
            cout << "----> UpdatePerformance_ParticleDensity_nParticles:" << endl;
            cout << "------> initialDensity:    " << std::fixed << std::setprecision(2) << getInitialDensity() << endl;
            cout << "------> traceStrain:       " << std::fixed << std::setprecision(2) << getTraceStrain() << endl;
            cout << "------> expectedDensity:   " << std::fixed << std::setprecision(2) << getExpectedDensity() << " (calculated)"<< endl;
            cout << "----> UpdatePerformance_ParticleStress_nParticles:" << endl;
            cout << "------> E:                 " << std::fixed << std::setprecision(2) << getE() << endl;
            cout << "------> nu:                " << std::fixed << std::setprecision(2) << getNu() << endl;
            cout << "------> epsilon0:          " << std::fixed << std::setprecision(2) << getEpsilon0() << endl;
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

            readGlobalConfiguration();
            cout << "\n";

            readSpecificConfiguration();
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