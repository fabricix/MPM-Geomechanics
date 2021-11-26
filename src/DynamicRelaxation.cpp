/*
 * DynamicRelaxation.cpp
 *
 *  Created on: 4 de set de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "DynamicRelaxation.h"
#include "Model.h"

namespace DynamicRelaxation {

    double lastKineticEnergy=0.0;
    double currentKineticEnergy=0.0;
}

void DynamicRelaxation::setStaticSolution(vector<Body*>* bodies, int loopCounter) { 

    // verify the damping type
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

    // get bodies
    for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

        // get particles
        vector<Particle*>* particles = bodies->at(ibody)->getParticles();
        
        // for each particle
        #pragma omp parallel for reduction(+:currentKineticEnergy) shared(particles) private(bodies, ibody)
        for (size_t i = 0; i < particles->size(); ++i) {

            // only active particle can contribute
            if (!particles->at(i)->getActive()) { continue; }
            
            // get mass and velocity
            const double mass = particles->at(i)->getMass();
            const Vector3d velocity = particles->at(i)->getVelocity();

            // add kinetic energy
            currentKineticEnergy+=0.5*mass*(velocity.x()*velocity.x()+velocity.y()*velocity.y()+velocity.z()*velocity.z());
        }
    }

    // compute the kinetic energy increment
    double deltaKineticEnergy = currentKineticEnergy - lastKineticEnergy;

    // check if there are a peak
    if (deltaKineticEnergy < 0.0)
    {   
        // set new current
        currentKineticEnergy  = 0.0;

        // get bodies
        for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

            // get particles
            vector<Particle*>* particles = bodies->at(ibody)->getParticles();
            
            // for each particle
            #pragma omp parallel for shared(particles) private(bodies, ibody)
            for (size_t i = 0; i < particles->size(); ++i) {

                // only active particle can contribute
                if (!particles->at(i)->getActive()) { continue; }
                
                // set nul velocity
                particles->at(i)->setVelocity(Vector3d(0,0,0));
            }
        }
    }

    // update last kinetic energy
    lastKineticEnergy = currentKineticEnergy;
}
