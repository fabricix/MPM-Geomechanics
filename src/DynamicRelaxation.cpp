// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "DynamicRelaxation.h"
#include "Energy.h"
#include "Model.h"
#include <iostream>


void DynamicRelaxation::setStaticSolution(vector<Body*>* bodies, int loopCounter) { 

    // damping type verification
    if (ModelSetup::getDampingType() != ModelSetup::KINETIC_DYNAMIC_RELAXATION)
    {
        return;
    }

    // first iteration
    if (loopCounter == 1)
    {
        Energy::inst().setLastKineticEnergy(0.0);
        return;
    }

    // compute the kinetic energy increment
    double deltaKineticEnergy = Energy::inst().getCurrentKineticEnergy() - Energy::inst().getLastKineticEnergy();

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
        
        Energy::inst().setCurrentKineticEnergy(0.0);
    }

    // update last kinetic energy
    Energy::inst().setLastKineticEnergyAsCurrent();
}
