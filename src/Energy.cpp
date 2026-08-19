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
        const int np = static_cast<int>(particles->size());

        // compute kinetic energy contribution of each particle
#ifdef _OPENMP
        #pragma omp parallel for shared(particles) reduction(+:energy) 
#endif
        for (int i = 0; i < np; ++i) 
        {
            Particle* p = (*particles)[i];

            if (!p->getActive()) continue;
            
            const double mass = p->getMass();

            const Vector3d velocity = p->getVelocity();

            energy += 0.5 * mass * velocity.squaredNorm();
        }
        // set the current total kinetic energy
        setCurrentKineticEnergy(energy);
    }
}