// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Integration.h"

void Integration::nodalMomentum(Mesh* mesh, double dt) {

	// get mesh
	vector<Node*>* nodes = mesh->getNodes();

	// for each node
#ifdef _OPENMP
	#pragma omp parallel for shared(nodes, dt)
#endif
	for (int i = 0; i < static_cast<int>(nodes->size()); ++i) {

		if (!nodes->at(i)->getActive()){ continue; }
			
		// integrate nodal momentum
		nodes->at(i)->integrateMomentum(dt);
	}
}
