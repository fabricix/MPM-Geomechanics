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
		
		int contactBodyId = -1;
		int contactBodySlaveId = -1;
		
		for (int j = 0; j < nBodies; j++){
			if (contactMatrix[i][j] == 1) {
				if (contactBodyId >= 0) {
					contactBodySlaveId = j;
				}
				else 
				{
					contactBodyId = j;
				}
			}
			
		}
		Node* node = mesh->getNodes()->at(i);
		NodeContact* nodeContact = dynamic_cast<NodeContact*>(node);
		
		if (contactBodySlaveId >= 0) {

			nodeContact->setContactStatus(true);
			ModelSetup::setContactActive(true);
			nodeContact->setContactBodies(contactBodyId, contactBodySlaveId);
			
		}
		else
		{
			nodeContact->setContactStatus(false); 
			//nodeContact->setContactBodies(-1, -1);

		}
	}
}


void Contact::checkContact(Mesh* mesh, vector<Body*>* bodies) {
	ModelSetup::setContactActive(false);
	firstContactCheck(mesh, bodies);
}

void Contact::contactForce(Mesh* mesh, vector<Body*>* bodies, double dt) {
	
	int nNodes = mesh->getNumNodes();
	int nBodies = bodies->size();
	

	//for each grid node
	for (int iNode = 0; iNode < nNodes; iNode++) {

		Node* node = mesh->getNodes()->at(iNode);
		NodeContact* nodeContact = dynamic_cast<NodeContact*>(node);
		
		if (nodeContact->getContactStatus()) {
			//for each body
			int contactBodyId = nodeContact->getContactBodyId(0);
			int contactBodySlaveId = nodeContact->getContactBodyId(1);
		}

		


	}
}

