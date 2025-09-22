#ifndef CONFIGURATION_BENCHMARK_H_
#define CONFIGURATION_BENCHMARK_H_

#include <string>
#include <vector>
#include "Eigen/Core"

using Eigen::Vector3d;
using Eigen::Vector3i;
using namespace std;

namespace Configuration
{
    // Global variables - Default values
    inline int numParticles = 150000;
    inline int numThreads = 10;
    inline Vector3d particleSize = Vector3d::Ones();
    inline double particleMass = 1.0;
    inline Vector3d cellDimension = Vector3d::Ones();
    inline Vector3i numCells = Vector3i::Ones();
    inline int randomSeed = 42;

    // Update - UpdatePerformance_ParticleDensity_nParticles - Default values
    inline double initialDensity = 2000;
    inline double traceStrain = 0.05;
    inline double expectedDensity = 2000 / (1.0 + 0.05);

    // Update - ParticleStress_nParticles - Default values
    inline double E = 1000.0;
    inline double nu = 0.3;
    inline double epsilon0 = 0.01;

    // Interpolation01 - NodalMass_nParticles - Default values
    // (no specific variables needed)

    // Interpolation02 - NodalMomentum_nParticles - Variables
    inline Vector3d particleVelocity(1.0, 2.0, 3.0);

    // Interpolation03 - NodalInternalForce_nParticles - Default values
    // (no specific variables needed)

    // Interpolation04 - InterpolationPerformance_NodalExternalForce_nParticles - Default values
    inline Vector3d appliedForce(1.0, 2.0, 3.0);

    // Interpolation05 - InterpolationPerformance_ParticleStrainIncrement_nParticles - Default values
    inline double dt01 = 0.01;
    inline Vector3d nodalVelocity(1.0, 2.0, 3.0);

    // Interpolation06 - InterpolationPerformance_ParticleVorticityIncrement_nParticles - Default values
    inline double dt02 = 0.01;
    inline Vector3d omega(1.0, 2.0, 3.0);

    // Test flags
    inline bool interpolationTest = true;
    inline bool updateTest = true;

    int setup();
    void openConfigurationFile();
    void readGlobalConfiguration();
    void readSpecificConfiguration();
};

#endif // CONFIGURATION_BENCHMARK_H_
