/*
 * Contact.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Contact.h"

Contact::Contact() { }

Contact::~Contact() { }

void Contact::firstContactCheck(Mesh* mesh, vector<Body*>* bodies) {
	
	int nNodes = mesh->getNumNodes();
	int nBodies = bodies->size();

	vector<vector<int>> contactMatrix(nNodes, vector<int>(nBodies, 0));
	
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
				if (contributions->at(j).getWeight() > 0.0 and contactMatrix[nodeId][ibody] == 0) {
					contactMatrix[nodeId][ibody] = 1;
				}
			}
		}
	}
	
	// for each node
	for (int i = 0; i < nNodes; i++) {
		
		int contactBodyId = -1;
		int contactBodySlaveId = -1;
		
		for (int j = 0; j < nBodies; j++){
			if (contactMatrix[i][j] == 1) {
				if (contactBodyId >= 0) { //verifies if any bodies contributed to node i
					contactBodySlaveId = j;
				}
				else 
				{
					contactBodyId = j;
				}
			}
			
		}
		Node* node = mesh->getNodes()->at(i);
		
		if (contactBodySlaveId >= 0) {

			node->setContactStatus(true);
			ModelSetup::setContactActive(true);
			node->setContactBodies(contactBodyId, contactBodySlaveId);
			
		}
		else
		{
			node->setContactStatus(false); 
		}
	}
}

void Contact::secondContactCheck(Mesh* mesh, vector<Body*>* bodies) {
	int nNodes = mesh->getNumNodes();
	int nBodies = bodies->size();


	//for each body
	for (size_t ibody = 0; ibody < nBodies; ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		#pragma omp parallel for shared(particles, mesh)
		for (int i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			Vector3d pPosition = particles->at(i)->getPosition();

			// get the contribution nodes
			vector<Contribution>* contributions = particles->at(i)->getContributionNodes();

			for (int j = 0; j < contributions->size(); j++) {

				int nodeId = contributions->at(j).getNodeId();
				Node* node = mesh->getNodes()->at(nodeId);

				// check any weight for node
				if (contributions->at(j).getWeight() <= 0.0) { continue; }

				Vector3d nCoordinates = node->getCoordinates();

				// Particle-node vector
				Vector3d PNVector = pPosition - nCoordinates;
			
				if (node->getContactStatus()) {
					//check if the body is set as slave
					if (ibody == node->getContactBodyId(1)) {
						//get closest distance to slave body
						double d = node->getClosestParticleDistanceSlave();
						
						//get normal vector
						Vector3d n = -*node->getUnitNormalTotal();
						

						//particle-node distance
						double dPN = -n.dot(PNVector);

						if (dPN < 0) {
							int a = 0;
						}

						if (dPN < d) {
							//set closest distance to slave body
							node->setClosestParticleDistanceSlave(dPN);
						}
					}
					else {
						//get closest distance to slave body
						double d = node->getClosestParticleDistance();

						//get normal vector
						Vector3d n = *node->getUnitNormalTotal();


						//particle-node distance
						double dPN = -n.dot(PNVector);

						if (dPN < 0) {
							int a = 0;
						}

						if (dPN < d && dPN > 0) {
							//set closest distance to slave body
							node->setClosestParticleDistance(dPN);
						}
					}
				}
			}
		}
	}


	//for each grid node
	for (int iNode = 0; iNode < nNodes; iNode++) {

		Node* node = mesh->getNodes()->at(iNode);

		if (node->getContactStatus()) {
			node->setSecondContactStatus(true);
			//for each body
			int contactBodyIdA = node->getContactBodyId(0);
			int contactBodySlaveIdB = node->getContactBodyId(1);

			Vector3d momentumA = node->getMomentum();
			Vector3d momentumB = *node->getMomentumSlave();
			double massA = node->getMass();
			double massB = node->getMassSlave();

			Vector3d n = *node->getUnitNormalTotal();

			if (n.dot(massB*momentumA - massA*momentumB) < 0) {
				node->setSecondContactStatus(false);
			}
			else {
				// contact correction
				double cellDimension = mesh->getCellDimension()[0];
				double contactDistance = node->getClosestParticleDistance() + node->getClosestParticleDistanceSlave();
				if (contactDistance > 0.5 * cellDimension) {
					node->setSecondContactStatus(false);
				}
				else {
					int a = 1;
				}
			}
			
		}
	}

}


void Contact::checkContact(Mesh* mesh, vector<Body*>* bodies) {
	ModelSetup::setContactActive(false);
	firstContactCheck(mesh, bodies);
}




void Contact::contactForce(Mesh* mesh, vector<Body*>* bodies, double dt) {
	
	secondContactCheck(mesh, bodies);

	int nNodes = mesh->getNumNodes();
	int nBodies = bodies->size();
	

	//for each grid node
	for (int iNode = 0; iNode < nNodes; iNode++) {

		Node* node = mesh->getNodes()->at(iNode);
		
		if (node->getSecondContactStatus()) {
			if (!ModelSetup::getSecondContactActive())
			{
				ModelSetup::setSecondContactActive(true);
			}
			double massA = node->getMass();
			double massB = node->getMassSlave();
			Vector3d momentumA = node->getVelocity();
			Vector3d momentumB = *node->getVelocitySlave();


			Vector3d f = (massA*momentumB - massB*momentumA) / (massA + massB) / dt;
			
			node->setContactForce(f);

			//add the contact force to the total force
			node->addContactForce();
		}

	}
}

void Contact::setParticlesInContact(Mesh* mesh, vector<Body*>* bodies) {

	int nNodes = mesh->getNumNodes();
	int nBodies = bodies->size();

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
				Node* node = mesh->getNodes()->at(nodeId);
				
				if (node->getContactStatus()) {
					particles->at(i)->setContact(true);
				}
			}
		}
	}
}


void Contact::resetParticlesInContact(Mesh* mesh, vector<Body*>* bodies) {

	int nNodes = mesh->getNumNodes();
	int nBodies = bodies->size();

	for (size_t ibody = 0; ibody < nBodies; ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
        #pragma omp parallel for shared(particles, mesh)
		for (int i = 0; i < particles->size(); ++i) {
			particles->at(i)->setContact(false);
		}
	}

}
