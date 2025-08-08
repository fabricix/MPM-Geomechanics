// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "DynamicRelaxation.h"
#include "Energy.h"
#include "Model.h"

#include <iostream>

void DynamicRelaxation::setStaticSolution(vector<Particle*>* particles)
{ 
    // damping type verification
    if (ModelSetup::getDampingType() != ModelSetup::KINETIC_DYNAMIC_RELAXATION) {
        return;
    }

    // compute the kinetic energy increment
    double deltaKineticEnergy = Energy::deltaKineticEnergy();

    // check if there was a peak
    if (deltaKineticEnergy < 0.0)
    {   
        // set the particles velocity to zero because the static solution was reached or passed
        #pragma omp parallel for shared(particles)
        for (int i = 0; i < static_cast<int> (particles->size()); ++i) {

            // verify active particle
            if (!particles->at(i)->getActive()) { continue; }
            
            // set null velocity
            particles->at(i)->setVelocity(Vector3d(0,0,0));
        }
        
        Energy::setCurrentKineticEnergy(0.0);
    }

    // update last kinetic energy
    Energy::setLastKineticEnergy(Energy::getCurrentKineticEnergy());
}