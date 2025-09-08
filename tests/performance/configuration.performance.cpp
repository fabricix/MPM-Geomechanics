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
    string getTextFormatted(string str01, string str02, int spacing)
    {
        string spaces = "";
        int separation = spacing - str01.length();
        if (separation < 1) separation = 5;
        spaces += string(separation, ' ');
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << str02;
        str02 = stream.str();
        return str01 + spaces + str02;
    }
}

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
    unordered_map<string, bool> jsonFields = {

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

    // Helper function to print text in blue
    string printInBlue(string text) { return "\033[34m" + text + "\033[0m"; }

    // Check if a key exists in the JSON file
    bool exists(json jsonFile, string key)
    {
        bool existsInConfigurationFile = false;
        bool existsInMap = false;
        try
        {
            if (jsonFields.find(key) != jsonFields.end()) jsonFields[key] = true;

            existsInConfigurationFile = jsonFile.contains(key);
            if (!existsInConfigurationFile)
            {
                cerr << "--> [INFO] A configuration key was not found in the json file: [" << key << "]" << endl;
                return false;
            }
            return existsInConfigurationFile;
        }
        catch (const std::exception& e)
        {
            cerr << "--> [WARNING] An error occurred while checking configuration key: [" << key << "]. Reason: " << e.what() << endl;
            cerr << "--> [WARNING] Please check your configuration file." << endl;
            return false;
        }
    }


    // Print Vector for different types
    template <typename T,
        typename = std::enable_if_t<
        std::is_same_v<T, Eigen::Vector3d> ||
        std::is_same_v<T, Eigen::Vector3i>>>
        string printVector(const T& vector, const string& key)
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2);
        out << "[" << vector[0] << ", " << vector[1] << ", " << vector[2] << "]";

        return Spaces::getTextFormatted(out.str(),
            (jsonFields[key] && !usingDefaultConfigurationFile ? "" : printInBlue("(default)")),
            PRINT_SPACES
        );
    }

    // Print value for different types
    template <typename T,
        typename = std::enable_if_t<
        std::is_same_v<T, int> ||
        std::is_same_v<T, float> ||
        std::is_same_v<T, double> ||
        std::is_same_v<T, bool>>>
        string printValue(const T& value, const string& key)
    {
        std::ostringstream out;
        out << std::fixed;
        if constexpr (std::is_same_v<T, bool>) { out << std::boolalpha; }
        else { out << std::setprecision(2); }
        out << value;

        return Spaces::getTextFormatted(out.str(),
            (jsonFields[key] && !usingDefaultConfigurationFile ? "" : printInBlue("(default)")),
            PRINT_SPACES
        );
    }

    // Helper function to format key printing
    string printKeys(string key01, string key02, string key03)
    {
        string fullKey = "";
        if (!key01.empty()) fullKey += printInBlue("[" + key01 + "]");
        if (!key02.empty()) fullKey += static_cast<string>(fullKey.empty() ? "" : "->") + printInBlue("[" + key02 + "]");
        if (!key03.empty()) fullKey += static_cast<string>(fullKey.empty() ? "" : "->") + printInBlue("[" + key03 + "]");
        return fullKey;
    }

    // Template function to get value from JSON with error handling
    template <typename T>
    T getValueFromJson(T defaultValue, string key01, string key02, string key03)
    {
        json configurationReference = configuration;
        if (!key01.empty() && !exists(configuration, key01))
        {
            cerr << "--> [INFO] Keys not Found: " << printKeys(key01, key02, key03) << ". Using default value." << endl;
            return defaultValue;
        }
        if (!key01.empty()) configurationReference = configurationReference[key01];

        if (!key02.empty() && !exists(configurationReference, key02))
        {
            cerr << "--> [INFO] Keys not Found: " << printKeys(key01, key02, key03) << ". Using default value." << endl;
            return defaultValue;
        }
        if (!key02.empty()) configurationReference = configurationReference[key02];

        if (!key03.empty() && !exists(configurationReference, key03))
        {
            cerr << "--> [INFO] Keys not Found: " << printKeys(key01, key02, key03) << ". Using default value." << endl;
            return defaultValue;
        }
        if (!key03.empty()) configurationReference = configurationReference[key03];

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
            cout << "---->" << printInBlue("numParticles") << ":        " << printValue(numParticles, "numParticles") << endl;
            cout << "---->" << printInBlue("numThreads") << ":          " << printValue(numThreads, "numThreads") << endl;
            cout << "---->" << printInBlue("particleSize") << ":        " << printVector(particleSize, "particleSize") << endl;
            cout << "---->" << printInBlue("particleMass") << ":        " << printValue(particleMass, "particleMass") << endl;
            cout << "---->" << printInBlue("cellDimension") << ":       " << printVector(cellDimension, "cellDimension") << endl;
            cout << "---->" << printInBlue("numCells") << ":            " << printVector(numCells, "numCells") << endl;
            cout << "---->" << printInBlue("randomSeed") << ":          " << printValue(randomSeed, "randomSeed") << endl;
            cout << "--> Test Flags" << endl;
            cout << "---->" << printInBlue("interpolationTest") << ":   " << printValue(interpolationTest, "interpolationTest") << endl;
            cout << "---->" << printInBlue("updateTest") << ":          " << printValue(updateTest, "updateTest") << endl;
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
            cout << "------> " << printInBlue("initialDensity") << ":    " << printValue(initialDensity, "initialDensity") << endl;
            cout << "------> " << printInBlue("traceStrain") << ":       " << printValue(traceStrain, "traceStrain") << endl;
            cout << "------> " << printInBlue("expectedDensity") << ":   " << printValue(expectedDensity, "expectedDensity") << " (calculated)" << endl;
            cout << "----> Performance Test: UpdatePerformance_ParticleStress_nParticles:" << endl;
            cout << "------> " << printInBlue("E") << ":                 " << printValue(E, "E") << endl;
            cout << "------> " << printInBlue("nu") << ":                " << printValue(nu, "nu") << endl;
            cout << "------> " << printInBlue("epsilon0") << ":          " << printValue(epsilon0, "epsilon0") << endl;
            cout << "----> Performance Test: Interpolation_NodalMomentum_nParticles:" << endl;
            cout << "------> " << printInBlue("particleVelocity") << ":  " << printVector(particleVelocity, "particleVelocity") << endl;
            cout << "----> Performance Test: Interpolation_NodalExternalForce_nParticles:" << endl;
            cout << "------> " << printInBlue("appliedForce") << ":      " << printVector(appliedForce, "appliedForce") << endl;
            cout << "----> Performance Test: Interpolation_ParticleStrainIncrement_nParticles:" << endl;
            cout << "------> " << printInBlue("dt01") << ":              " << printValue(dt01, "dt01") << endl;
            cout << "------> " << printInBlue("nodalVelocity") << ":     " << printVector(nodalVelocity, "nodalVelocity") << endl;
            cout << "----> Performance Test: Interpolation_ParticleVorticityIncrement_nParticles:" << endl;
            cout << "------> " << printInBlue("dt02") << ":              " << printValue(dt02, "dt02") << endl;
            cout << "------> " << printInBlue("omega") << ":             " << printVector(omega, "omega") << endl;
        }
        catch (const std::exception& e)
        {
            cerr << "--> [ERROR] Failed at step: " << step << ". Please check the json file." << endl;
            cerr << "--> [ERROR] Error reading configuration:" << e.what() << endl;
            throw (0);
        }
    }

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
};