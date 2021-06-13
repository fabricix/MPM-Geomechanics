/*
 * Integration.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Integration.h"

void Integration::nodalMomentum(Mesh* mesh, double dt){

	// get mesh
	vector<Node>* nodes = mesh->getNodes();

	// for each node
	for (size_t i = 0; i < nodes->size(); ++i) {

		// get node handle
		Node& nodeI = nodes->at(i);

		if (nodeI.getActive()){
			
			// integrate nodal momentum
			nodeI.integrateMomentum(dt);
		}
	}
}
