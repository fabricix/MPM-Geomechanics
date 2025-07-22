// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "DynamicRelaxation.h"
#include "Model.h"
#include <iostream>

namespace DynamicRelaxation
{
    double currentKineticEnergy = 0.0;
    double lastKineticEnergy = 0.0;

    double getCurrentKineticEnergy() { return currentKineticEnergy; }
    void setCurrentKineticEnergy(double energy) { currentKineticEnergy = energy; }
}

void DynamicRelaxation::computeKineticEnergy(vector<Body*>* bodies)
{
    // initial value for energy
    double energy = 0.0;

    // for each body
    for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

        // get particles
        vector<Particle*>* particles = bodies->at(ibody)->getParticles();
        
        // for each particle
        #pragma omp parallel for reduction(+:energy) shared(particles)
        for (int i = 0; i < static_cast<int> (particles->size()); ++i) {

            // verify active particle
            if (!particles->at(i)->getActive()) { continue; }
            
            // get mass and velocity
            const double mass = particles->at(i)->getMass();
            const Vector3d velocity = particles->at(i)->getVelocity();

            // compute the particle kinetic energy contribution
            energy += 0.5 * mass * (velocity.x() * velocity.x() + velocity.y() * velocity.y() + velocity.z() * velocity.z());
        }
    }

    setCurrentKineticEnergy(energy);
}

void DynamicRelaxation::setStaticSolution(vector<Body*>* bodies, int loopCounter) { 

    // damping type verification
    if (ModelSetup::getDampingType() != ModelSetup::KINETIC_DYNAMIC_RELAXATION)
    {
        return;
    }

    // first iteration
    if (loopCounter == 1)
    {
        lastKineticEnergy = 0.0;
        return;
    }

    // compute the kinetic energy increment
    double deltaKineticEnergy = getCurrentKineticEnergy() - lastKineticEnergy;

    // check if there was a peak
    if (deltaKineticEnergy < 0.0)
    {   
        // set null velocity in the peak energy condition
        for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

            // get particles
            vector<Particle*>* particles = bodies->at(ibody)->getParticles();
            
            // for each particle
            #pragma omp parallel for shared(particles)
            for (int i = 0; i < static_cast<int> (particles->size()); ++i) {

                // verify active particle
                if (!particles->at(i)->getActive()) { continue; }
                
                // set null velocity
                particles->at(i)->setVelocity(Vector3d(0,0,0));
            }
        }

        // set current kinetic energy
        setCurrentKineticEnergy(0.0);
    }

    // update last kinetic energy
    lastKineticEnergy = getCurrentKineticEnergy();
}
