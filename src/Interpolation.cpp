/*
 * Interpolation.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Interpolation.h"
#include "Contribution.h"
#include "Particle.h"

Interpolation::Interpolation() {
	// TODO Auto-generated constructor stub

}

Interpolation::~Interpolation() {
	// TODO Auto-generated destructor stub
}

bool Interpolation::interpolateMassToNodes(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// In each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// add the weighted particle mass.
			nodes->at(contribution->at(j).nodeId).addMass(particles.at(i).getMass()*contribution->at(j).weight);
		}
	}
	return true;
}