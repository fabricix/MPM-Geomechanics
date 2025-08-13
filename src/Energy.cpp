// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Energy.h"

namespace Energy
{
    double currentKineticEnergy = 0.0; //!< Current kinetic energy
    double lastKineticEnergy = 0.0; //!< Last kinetic energy

    double getCurrentKineticEnergy() { return currentKineticEnergy; } //!< Get current kinetic energy
    void setCurrentKineticEnergy(double energy) { currentKineticEnergy = energy; } //!< Set current kinetic energy
   
    double getLastKineticEnergy() { return lastKineticEnergy; } //!< Get last kinetic energy
    void setLastKineticEnergy(double energy) { lastKineticEnergy = energy; } //!< Set last kinetic energy
    
    double deltaKineticEnergy() { return (currentKineticEnergy - lastKineticEnergy); } //!< Set last kinetic energy as current

    void computeKineticEnergy(vector<Particle*>* particles)
    {
        // initial value for energy
        double energy = 0.0;

        // for each particle
#ifdef _OPENMP
        #pragma omp parallel for reduction(+:energy) shared(particles)
#endif
        for (int i = 0; i < static_cast<int>(particles->size()); ++i) 
        {
            // verify active particle
            if (!particles->at(i)->getActive()) { continue; }

            // get mass and velocity
            const double mass = particles->at(i)->getMass();
            const Vector3d velocity = particles->at(i)->getVelocity();

            // compute the particle kinetic energy contribution
            energy += 0.5 * mass * (velocity.x() * velocity.x() + velocity.y() * velocity.y() + velocity.z() * velocity.z());
        }
           
        setCurrentKineticEnergy(energy);
    }
}