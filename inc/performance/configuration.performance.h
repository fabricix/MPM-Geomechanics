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
    int getNumParticles();
    int getNumThreads();
    Vector3d getParticleSize();
    double getParticleMass();
    Vector3d getCellDimension();
    Vector3i getNumCells();
    int getRandomSeed();

    int setup();
};

#endif // CONFIGURATION_PERFORMANCE_H_
