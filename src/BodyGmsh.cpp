// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Body/BodyGmsh.h"
#include "Particle/Particle.h"
#include "Model.h"

BodyGmsh::BodyGmsh(
					const std::string& mesh_file,
					std::map<std::string, int> physical_to_material,
                   	int nppc_tet,
                   	int nppc_hex)
				   :
				    meshFile(mesh_file),
				    physicalToMaterial(std::move(physical_to_material)),
					nppcTet(nppc_tet),
					nppcHex(nppc_hex),
					materialId(-1) 
{
    // empty
}

BodyGmsh::~BodyGmsh() { }


void BodyGmsh::create(Mesh& mesh, Material* material)
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