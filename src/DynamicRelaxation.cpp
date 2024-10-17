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
}

double DynamicRelaxation::computeKineticEnergy(vector<Body*>* bodies)
{
    // initial value for energy
    double currentKineticEnergy (0.0);

    // for each body
    for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

        // get particles
        vector<Particle*>* particles = bodies->at(ibody)->getParticles();
        
        // for each particle
        #pragma omp parallel for reduction(+:currentKineticEnergy) shared(particles)
        for (int i = 0; i < particles->size(); ++i) {

            // verify active particle
            if (!particles->at(i)->getActive()) { continue; }
            
            // get mass and velocity
            const double mass = particles->at(i)->getMass();
            const Vector3d velocity = particles->at(i)->getVelocity();

            // compute the particle kinetic energy contribution
            currentKineticEnergy += 0.5*mass*(velocity.x()*velocity.x()+velocity.y()*velocity.y()+velocity.z()*velocity.z());
        }
    }

    return currentKineticEnergy;
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
        
    // current kinetic energy
    double currentKineticEnergy = computeKineticEnergy(bodies);

    // compute the kinetic energy increment
    double deltaKineticEnergy = currentKineticEnergy - lastKineticEnergy;

    // check if there was a peak
    if (deltaKineticEnergy < 0.0)
    {   
        // set null velocity in the peak energy condition
        for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

            // get particles
            vector<Particle*>* particles = bodies->at(ibody)->getParticles();
            
            // for each particle
            #pragma omp parallel for shared(particles)
            for (int i = 0; i < particles->size(); ++i) {

                // verify active particle
                if (!particles->at(i)->getActive()) { continue; }
                
                // set null velocity
                particles->at(i)->setVelocity(Vector3d(0,0,0));
            }
        }

        // set current kinetic energy
        currentKineticEnergy = 0.0;
    }

    // update last kinetic energy
    lastKineticEnergy = currentKineticEnergy;
}
