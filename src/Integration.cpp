/*
 * Integration.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Integration.h"

Integration::Integration() {
	// TODO Auto-generated constructor stub

}

Integration::~Integration() {
	// TODO Auto-generated destructor stub
}

void Integration::nodalMomentum(Mesh& mesh,double dt){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	for (size_t i = 0; i < nodes->size(); ++i)
	{
		nodes->at(i).setMomentum(nodes->at(i).getMomentum()+nodes->at(i).getTotalForce()*dt);
	}
}
