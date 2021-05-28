/*
 * Integration.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Integration.h"

void Integration::nodalMomentum(Mesh& mesh,double dt){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	for (size_t i = 0; i < nodes->size(); ++i){

		Node& nodeI = nodes->at(i);
		
		if (nodeI.getActive()){
			
			nodeI.setMomentum(nodeI.getMomentum()+nodeI.getTotalForce()*dt);
		}
	}
}
