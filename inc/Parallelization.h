#ifndef PARALLELIZATION_H_
#define PARALLELIZATION_H_

using namespace std;
#include <vector>
#include "Mesh.h"

class Particle;

namespace Parallelization
{
  void calculateParticlesPerThread(vector<vector<Particle*>*>& particlesPerThread, vector<Particle*>& particles, int factor, Mesh* mesh);
  void calculateInterfaceParticles(vector<vector<Particle*>*>& particlesPerThread, vector<Particle*>& particles, Mesh* mesh);
}

#endif // PARALLELIZATION_H_