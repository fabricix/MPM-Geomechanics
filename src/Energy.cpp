#include "Energy.h"

void Energy::computeKineticEnergy(vector<Body*>* bodies)
{
  // initial value for energy
  double energy = 0.0;

  // for each body
  for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

    // get particles
    vector<Particle*>* particles = bodies->at(ibody)->getParticles();

    // for each particle
    #pragma omp parallel for reduction(+:energy) shared(particles)
    for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

      // verify active particle
      if (!particles->at(i)->getActive()) { continue; }

      // get mass and velocity
      const double mass = particles->at(i)->getMass();
      const Vector3d velocity = particles->at(i)->getVelocity();

      // compute the particle kinetic energy contribution
      energy += 0.5 * mass * (velocity.x() * velocity.x() + velocity.y() * velocity.y() + velocity.z() * velocity.z());
    }
  }

  Energy::inst().setCurrentKineticEnergy(energy);
}