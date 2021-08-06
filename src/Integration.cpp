/*
 * Integration.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Integration.h"

void Integration::nodalMomentum(Mesh* mesh, double dt) {

	// get mesh
	vector<Node*>* nodes = mesh->getNodes();

	// for each node
	#pragma omp parallel for shared(nodes, dt) private(mesh)
	for (size_t i = 0; i < nodes->size(); ++i) {

		if (!nodes->at(i)->getActive()){ continue; }
			
		// integrate nodal momentum
		nodes->at(i)->integrateMomentum(dt);
	}
}
