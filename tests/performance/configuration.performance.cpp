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

namespace Configuration
{
    const int PRINT_SPACES = 25;
    bool usingDefaultConfigurationFile = false;
    json configuration;
    string jsonFileName = "test-configuration.json";
    json defaultConfigurationFile = {
        {"global", {
            {"numParticles", static_cast<int>(numParticles)},
            {"numThreads", static_cast<int>(numThreads)},
            {"cellDimension", {static_cast<int>(cellDimension[0]), static_cast<int>(cellDimension[1]), static_cast<int>(cellDimension[2])}},
            {"numCells", {static_cast<int>(numCells[0]), static_cast<int>(numCells[1]), static_cast<int>(numCells[2])}},
            {"randomSeed", static_cast<int>(randomSeed)},
            {"interpolationTest", static_cast<bool>(interpolationTest)},
            {"updateTest", static_cast<bool>(updateTest)}
        }},
        {"update", {
            {"UpdatePerformance_ParticleDensity_nParticles", {
                {"initialDensity", static_cast<double>(initialDensity)},
                {"traceStrain", static_cast<double>(traceStrain)}
            }},
            {"ParticleStress_nParticles", {
                {"E", static_cast<double>(E)},
                {"nu", static_cast<double>(nu)},
                {"epsilon0", static_cast<double>(epsilon0)}
            }}
        }},
        {"interpolation", {
            {"NodalMass_nParticles", {
                {"comment", "No specific parameters for this test"}
            }},
            {"NodalMomentum_nParticles", {
                {"particleVelocity", {static_cast<double>(particleVelocity[0]), static_cast<double>(particleVelocity[1]), static_cast<double>(particleVelocity[2])}}
            }},
            {"NodalInternalForce_nParticles", {
                {"comment", "No specific parameters for this test"}
            }},
            {"NodalExternalForce_nParticles", {
                {"appliedForce", {static_cast<double>(appliedForce[0]), static_cast<double>(appliedForce[1]), static_cast<double>(appliedForce[2])}}
            }},
            {"ParticleStrainIncrement_nParticles", {
                {"dt01", static_cast<double>(dt01)},
                {"nodalVelocity", {static_cast<double>(nodalVelocity[0]), static_cast<double>(nodalVelocity[1]), static_cast<double>(nodalVelocity[2])}}
            }},
            {"ParticleVorticityIncrement_nParticles", {
                {"dt02", static_cast<double>(dt02)},
                {"omega", {static_cast<double>(omega[0]), static_cast<double>(omega[1]), static_cast<double>(omega[2])}}
            }}
        }}
    };

    // Track which fields were found in the JSON file
    unordered_map<string, bool> fields = {
        {"numParticles",  false},
        {"numThreads",  false},
        {"cellDimension",  false},
        {"numCells",  false},
        {"randomSeed",  false},
        {"UpdatePerformance_ParticleDensity_nParticles",  false},
        {"initialDensity",  false},
        {"traceStrain",  false},
        {"ParticleStress_nParticles",  false},
        {"E",  false},
        {"nu",  false},
        {"epsilon0",  false},
        {"NodalMass_nParticles",  false},
        {"NodalMomentum_nParticles",  false},
        {"particleVelocity",  false},
        {"NodalInternalForce_nParticles",  false},
        {"NodalExternalForce_nParticles",  false},
        {"appliedForce",  false},
        {"ParticleStrainIncrement_nParticles",  false},
        {"dt01",  false},
        {"nodalVelocity",  false},
        {"ParticleVorticityIncrement_nParticles",  false},
        {"dt02",  false},
        {"omega",  false},
        {"update",  false},
        {"interpolation",  false},
        {"interpolationTest",  false},
        {"updateTest",  false}
    };

    // Setup function to initialize configuration
    int setup()
    {
        cout << "------------------------" << endl;
        cout << "SETUP CONFIGURATION" << endl;
        cout << "\n";

        int status = 0;
        try
        {
            openConfigurationFile();
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

    // Helper function to print text in blue
    string textInBlue(string text) { return "\033[34m" + text + "\033[0m"; }

    string getTextFormatted(string str01, string str02)
    {
        int separation = PRINT_SPACES - str01.length();
        if (separation < 1) separation = 5;
        return str01 + string(separation, ' ') + str02;
    }

    // Check if a key exists in the JSON file
    bool exists(json configurationFile, string key)
    {
        if (key.empty()) return false;
        try
        {
            if (configurationFile.contains(key))
            {
                fields[key] = true;
                return true;
            }
            else
            {
                return false;
            }
        }
        catch (const std::exception& e)
        {
            cerr << "--> [WARNING] An error occurred while checking configuration key: [" << key << "]. Reason: " << e.what() << endl;
            cerr << "--> [WARNING] Please check your configuration file." << endl;
            return false;
        }
    }

    template <typename T,
        typename = std::enable_if_t<
        std::is_same_v<T, int> ||
        std::is_same_v<T, float> ||
        std::is_same_v<T, double> ||
        std::is_same_v<T, bool> ||
        std::is_same_v<T, Eigen::Vector3d> ||
        std::is_same_v<T, Eigen::Vector3i>>>
    string valueAsString(const T& value, const string& key)
    {
        std::ostringstream out;
        out ;

        if constexpr (std::is_same_v<T, bool>)
        {
            out << std::boolalpha << value;
        }
        else if constexpr (std::is_same_v<T, Eigen::Vector3d> || std::is_same_v<T, Eigen::Vector3i>)
        {
            out << "[" << value[0] << ", " << value[1] << ", " << value[2] << "]";
        }
        else if constexpr (std::is_same_v<T, int>)
        {
            out << value;
        }
        else // float or double
        {
            out << std::setprecision(2) << std::scientific << value;
        }

        return getTextFormatted(
            out.str(),
            (fields[key] && !usingDefaultConfigurationFile ? "" : textInBlue("(default)"))
        );
    }

    // Helper function to format key printing
    string keysAsString(vector<string> keys)
    {
        string fullKey = "";
        for (const auto& key : keys)
        {
            if (!key.empty())
            {
                fullKey += static_cast<string>(fullKey.empty() ? "" : "->") + textInBlue("[" + key + "]");
            }
        }
        return fullKey;
    }

    // Template function to get value from JSON with error handling
    template <typename T>
    T getValueFromJson(T defaultValue, string key01, string key02, string key03)
    {
        json configurationReference = configuration;
        vector<string> keys = { key01, key02, key03 };
        string key = "";

        // Navigate through the JSON structure based on provided keys
        for (int k = 0; k < 3; k++) {
            key = keys[k];

            if (key.empty()) continue;

            // If the key does not exist in the configuration, return the default value
            if (exists(configurationReference, key))
            {
                configurationReference = configurationReference[key];
            }
            else
            {
                cerr << "--> [INFO] Keys not Found: " << keysAsString(keys) << ". Using default value." << endl;
                return defaultValue;
            }
        }

        if constexpr (std::is_same_v<T, Vector3d>)
        {
            if (configurationReference.size() != 3) return defaultValue;
            return Vector3d(
                static_cast<double>(configurationReference[0]),
                static_cast<double>(configurationReference[1]),
                static_cast<double>(configurationReference[2])
            );
        }
        else if constexpr (std::is_same_v<T, Vector3i>)
        {
            if (configurationReference.size() != 3) return defaultValue;
            return Vector3i(
                static_cast<int>(configurationReference[0]),
                static_cast<int>(configurationReference[1]),
                static_cast<int>(configurationReference[2])
            );
        }
        else { return configurationReference.get<T>(); }
    }

    // Overloaded functions for different number of keys
    template <typename T>
    T getValueFromJson(T defaultValue, string key01, string key02) { return getValueFromJson(defaultValue, key01, key02, ""); }

    // Overloaded function for single key
    template <typename T>
    T getValueFromJson(T defaultValue, string key01) { return getValueFromJson(defaultValue, key01, "", ""); }

    // Open and parse the JSON configuration file
    void openConfigurationFile()
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

        ifstream file(jsonFileName);
        if (!file.is_open()) {
            cerr << "--> [INFO] There is no configuration file found: " << jsonFileName << endl;
            cerr << "--> [INFO] Using default configuration values." << endl;
            usingDefaultConfigurationFile = true;
            configuration = defaultConfigurationFile;
        }
        else
        {
            cout << "--> File opened successfully: " << jsonFileName << endl;
            file >> configuration;
        }
    }

    // Read global configuration parameters
    void readGlobalConfiguration()
    {
        cout << "> Read global configuration..." << endl;

        string step;
        string expected;
        try
        {
            // Global variables
            step = "setting num_particles";
            numParticles = getValueFromJson(numParticles, "global", "numParticles");

            step = "setting num_threads";
            numThreads = getValueFromJson(numThreads, "global", "numThreads");

            step = "setting cell_dimension";
            cellDimension = getValueFromJson(cellDimension, "global", "cellDimension");

            step = "setting num_cells";
            numCells = getValueFromJson(numCells, "global", "numCells");

            step = "setting random_seed";
            randomSeed = getValueFromJson(randomSeed, "global", "randomSeed");

            step = "setting interpolationTest";
            interpolationTest = getValueFromJson(interpolationTest, "global", "interpolationTest");

            step = "setting updateTest";
            updateTest = getValueFromJson(updateTest, "global", "updateTest");


            cout << "--> Global Configuration read successfully." << endl;
            cout << "--> Displaying configuration values:" << endl;
            cout << "---->" << textInBlue("numParticles") << ":        " << valueAsString(numParticles, "numParticles") << endl;
            cout << "---->" << textInBlue("numThreads") << ":          " << valueAsString(numThreads, "numThreads") << endl;
            cout << "---->" << textInBlue("particleSize") << ":        " << valueAsString(particleSize, "particleSize") << endl;
            cout << "---->" << textInBlue("particleMass") << ":        " << valueAsString(particleMass, "particleMass") << endl;
            cout << "---->" << textInBlue("cellDimension") << ":       " << valueAsString(cellDimension, "cellDimension") << endl;
            cout << "---->" << textInBlue("numCells") << ":            " << valueAsString(numCells, "numCells") << endl;
            cout << "---->" << textInBlue("randomSeed") << ":          " << valueAsString(randomSeed, "randomSeed") << endl;
            cout << "--> Test Flags" << endl;
            cout << "---->" << textInBlue("interpolationTest") << ":   " << valueAsString(interpolationTest, "interpolationTest") << endl;
            cout << "---->" << textInBlue("updateTest") << ":          " << valueAsString(updateTest, "updateTest") << endl;
        }
        catch (const std::exception& e)
        {
            cerr << "--> [ERROR] Failed at step: " << step << ". Please check the json file." << endl;
            cerr << "--> [ERROR] Error reading configuration:" << e.what() << endl;
            throw (0);
        }
    }

    // Read specific configuration parameters for performance tests
    void readSpecificConfiguration()
    {
        cout << "> Read specific configuration..." << endl;
        string step;

        try
        {
            // --> Update - UpdatePerformance_ParticleDensity_nParticles
            step = "setting initialDensity";
            initialDensity = getValueFromJson(initialDensity, "update", "UpdatePerformance_ParticleDensity_nParticles", "initialDensity");

            step = "setting traceStrain";
            traceStrain = getValueFromJson(traceStrain, "update", "UpdatePerformance_ParticleDensity_nParticles", "traceStrain");

            // The expected density is calculated based on the initial density and trace strain
            step = "setting expectedDensity";
            expectedDensity = initialDensity / (1.0 + traceStrain);

            // --> Update - ParticleStress_nParticles
            step = "setting E";
            E = getValueFromJson(E, "update", "ParticleStress_nParticles", "E");

            step = "setting nu";
            nu = getValueFromJson(nu, "update", "ParticleStress_nParticles", "nu");

            step = "setting epsilon0";
            epsilon0 = getValueFromJson(epsilon0, "update", "ParticleStress_nParticles", "epsilon0");

            // --> Interpolation - NodalMomentum_nParticles
            step = "setting particleVelocity";
            particleVelocity = getValueFromJson(particleVelocity, "interpolation", "NodalMomentum_nParticles", "particleVelocity");

            // --> Interpolation - NodalExternalForce_nParticles
            step = "setting appliedForce";

            appliedForce = getValueFromJson(appliedForce, "interpolation", "NodalExternalForce_nParticles", "appliedForce");

            // --> Interpolation - ParticleStrainIncrement_nParticles
            step = "setting dt01";
            dt01 = getValueFromJson(dt01, "interpolation", "ParticleStrainIncrement_nParticles", "dt01");

            step = "setting nodalVelocity";
            nodalVelocity = getValueFromJson(nodalVelocity, "interpolation", "ParticleStrainIncrement_nParticles", "nodalVelocity");

            // --> Interpolation - ParticleVorticityIncrement_nParticles
            step = "setting dt02";
            dt02 = getValueFromJson(dt02, "interpolation", "ParticleVorticityIncrement_nParticles", "dt02");

            step = "setting omega";
            omega = getValueFromJson(omega, "interpolation", "ParticleVorticityIncrement_nParticles", "omega");

            // Finalizing
            step = "finalizing configuration";

            // Displaying configuration values
            cout << "--> Specific Configuration read successfully." << endl;
            cout << "--> Displaying configuration values:" << endl;
            cout << "----> Performance Test: UpdatePerformance_ParticleDensity_nParticles:" << endl;
            cout << "------> " << textInBlue("initialDensity") << ":    " << valueAsString(initialDensity, "initialDensity") << endl;
            cout << "------> " << textInBlue("traceStrain") << ":       " << valueAsString(traceStrain, "traceStrain") << endl;
            cout << "------> " << textInBlue("expectedDensity") << ":   " << valueAsString(expectedDensity, "expectedDensity") << " (calculated)" << endl;
            cout << "----> Performance Test: UpdatePerformance_ParticleStress_nParticles:" << endl;
            cout << "------> " << textInBlue("E") << ":                 " << valueAsString(E, "E") << endl;
            cout << "------> " << textInBlue("nu") << ":                " << valueAsString(nu, "nu") << endl;
            cout << "------> " << textInBlue("epsilon0") << ":          " << valueAsString(epsilon0, "epsilon0") << endl;
            cout << "----> Performance Test: Interpolation_NodalMomentum_nParticles:" << endl;
            cout << "------> " << textInBlue("particleVelocity") << ":  " << valueAsString(particleVelocity, "particleVelocity") << endl;
            cout << "----> Performance Test: Interpolation_NodalExternalForce_nParticles:" << endl;
            cout << "------> " << textInBlue("appliedForce") << ":      " << valueAsString(appliedForce, "appliedForce") << endl;
            cout << "----> Performance Test: Interpolation_ParticleStrainIncrement_nParticles:" << endl;
            cout << "------> " << textInBlue("dt01") << ":              " << valueAsString(dt01, "dt01") << endl;
            cout << "------> " << textInBlue("nodalVelocity") << ":     " << valueAsString(nodalVelocity, "nodalVelocity") << endl;
            cout << "----> Performance Test: Interpolation_ParticleVorticityIncrement_nParticles:" << endl;
            cout << "------> " << textInBlue("dt02") << ":              " << valueAsString(dt02, "dt02") << endl;
            cout << "------> " << textInBlue("omega") << ":             " << valueAsString(omega, "omega") << endl;
        }
        catch (const std::exception& e)
        {
            cerr << "--> [ERROR] Failed at step: " << step << ". Please check the json file." << endl;
            cerr << "--> [ERROR] Error reading configuration:" << e.what() << endl;
            throw (0);
        }
    }
};