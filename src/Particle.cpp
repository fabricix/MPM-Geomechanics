/*
 * Particle.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Particle.h"
#include "Mesh.h"

Particle::Particle() {

	id=0;
	materialId=0;
	cellId=0;
	bodyId=0;

	mass=0.0;
	density=0.0;
	volume=0.0;
	plasticStrain=0.0;
	initialVolume=0.0;
	porosity=0.0;

	position.setZero();
	velocity.setZero();
	momentum.setZero();
	externalForce.setZero();

	stress.setZero();
	strain.setZero();
}

Particle::~Particle() {
	// TODO Auto-generated destructor stub
}

bool Particle::updateContributionNodes(const Mesh &)
{
	return true;
}
