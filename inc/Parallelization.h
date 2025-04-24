#ifndef PARALLELIZATION_H_
#define PARALLELIZATION_H_

using namespace std;
#include <vector>
#include "Mesh/Mesh.h"

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
  void calculateInterfaceParticles(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread); //vector<vector<Particle*>*>& interfaceParticlesPerThread

  /// \brief Calculate the interface particles
  /// \param[in] mesh Mesh object containing the nodes and particles
	/// \param[in] particlesPerThread Actual particles per thread
  void interpolateMass(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread, int factor);

  void nodalMomentum(Mesh* mesh, vector<vector<Particle*>*>& particlesPerThread, int factor); 
}

#endif // PARALLELIZATION_H_