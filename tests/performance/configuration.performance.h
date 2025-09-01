#ifndef CONFIGURATION_PERFORMANCE_H_
#define CONFIGURATION_PERFORMANCE_H_

#include <string>
#include <vector>
#include "Eigen/Core"

using Eigen::Vector3d;
using Eigen::Vector3i;
using namespace std;

namespace Configuration
{
    // Gloabal variables
    int getNumParticles();
    int getNumThreads();
    Vector3d getParticleSize();
    double getParticleMass();
    Vector3d getCellDimension();
    Vector3i getNumCells();
    int getRandomSeed();

    // Update - UpdatePerformance_ParticleDensity_nParticles - Variables
    double getInitialDensity();
    double getTraceStrain();
    double getExpectedDensity();

    // Update - ParticleStress_nParticles - Variables
    double getE();
    double getNu();
    double getEpsilon0();

    // Get interpolation and update test flags
    bool getInterpolationTest();
    bool getUpdateTest();

    int setup();
};

#endif // CONFIGURATION_PERFORMANCE_H_
