/*
 * Loads.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Loads.h"
#include "Model.h"

namespace Loads{

	void setGravity(vector<Body*>& bodies) {

		// return if gravity is not activated
		if (!ModelSetup::getGravityActive()) { return; }

		// get gravity
		Vector3d gravity = ModelSetup::getGravity();

		// is two-phase calculations 
		bool isTwoPhase = ModelSetup::getTwoPhaseActive();

		// for each body
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

				// get particle handle
				Particle* particle = particles->at(i);

				// compute the gravitational force
				particle->addExternalForce(particle->getMass()*gravity);

				if (isTwoPhase)
				{
					// body force of fluid to solid phase
					particle->addExternalForce(particle->getMassFluid()*gravity);

					// body force of fluid in fluid phase
					particle->addExternalForceFluid(particle->getMassFluid()*gravity);
				}
			}
		}
	}
}
