// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

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
	// unused for now
	(void)mesh;

	// get particles
	vector<Particle*>* particles = Body::getParticles();

	// for each particle
	for (size_t i = 0; i < particles->size(); ++i) {

		// only active particle can contribute
		particles->at(i)->setMaterial(material);
	}
}

