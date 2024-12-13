/*
 * Contact.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Contact.h"
#include "Mesh/NodeContact.h"

Contact::Contact() { }

Contact::~Contact() { }

void Contact::firstContactCheck(Mesh* mesh, vector<Body*>* bodies) {
	
	int nNodes = mesh->getNumNodes();
	int nBodies = bodies->size();

	vector<vector<int>> contactMatrix(nNodes, vector<int>(nBodies, 0));
	
	//Esse trecho pode ser adicionado a Update::contributionNodes(mesh, bodies)
	// for each body
	for (size_t ibody = 0; ibody < nBodies; ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
	#pragma omp parallel for shared(particles, mesh)
		for (int i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// get the contribution nodes
			vector<Contribution>* contributions = particles->at(i)->getContributionNodes();

			for (int j = 0; j < contributions->size(); j++) {
				
				int nodeId = contributions->at(j).getNodeId();
				if (contributions->at(j).getWeight() > 0 and contactMatrix[nodeId][ibody] == 0) {
					contactMatrix[nodeId][ibody] = 1;
				}
			}
		}
	}
	

	for (int i = 0; i < nNodes; i++) {
		int contributionCont = 0;

		for (int j = 0; j < nBodies; j++){
			if (contactMatrix[i][j] == 1) {
				contributionCont++;
			}
			
		}
		if (contributionCont > 1) {
			Node*  node = mesh->getNodes()->at(i);
			NodeContact* nodeContact = dynamic_cast<NodeContact*>(node);
			nodeContact->setContactFlag(true);

		}
	}
}


void Contact::checkContact(Mesh* mesh, vector<Body*>* bodies) {
	firstContactCheck(mesh, bodies);

}

