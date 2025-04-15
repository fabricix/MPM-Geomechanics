#ifndef PARALLELIZATION_H_
#define PARALLELIZATION_H_

using namespace std;
#include <vector>
#include "Mesh.h"

class Particle;

namespace Parallelization
{
  /// \brief Calculate the particles per thread
  /// \param[in] mesh Mesh object containing the nodes and particles
  /// \param[in] particlesPerThread Actual particles per thread
  /// \param[in] particles List of particles to be distributed
  /// \param[in] factor The factor to determine the number of threads (2^factor)
  void calculateParticlesPerThread(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread, vector<Particle*>& particles, int factor);

  /// \brief Calculate the interface particles
  /// \param[in] mesh Mesh object containing the nodes and particles
	/// \param[in] particlesPerThread Actual particles per thread
  void calculateInterfaceParticles(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread);
}

#endif // PARALLELIZATION_H_