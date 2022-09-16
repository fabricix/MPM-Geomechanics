/*
 * BodyParticle.cpp
 *
 *  Created on: 15 de set. de 2022
 *      Author: fabricio
 */

#include "Body/BodyParticle.h"

BodyParticle::BodyParticle():Body() {

	this->materialId=-1;
}

BodyParticle::~BodyParticle() { }

/// \brief Create a body in space using particles
/// \param[in] &mesh Mesh reference
/// \param[in] *material Material pointer
void BodyParticle::create(Mesh& mesh, Material* material)
{
	// get particles
	vector<Particle*>* particles = Body::getParticles();

	// for each particle
	for (size_t i = 0; i < particles->size(); ++i) {

		// only active particle can contribute
		particles->at(i)->setMaterial(material);
	}
}

