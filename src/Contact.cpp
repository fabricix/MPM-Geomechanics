/*
 * Contact.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Contact.h"

Contact::Contact() { }

Contact::~Contact() { }

// First contact check
void Contact::firstContactCheck(Mesh* mesh, vector<Body*>* bodies) {
	
	// get number of nodes and bodies
	int nNodes = (int) mesh->getNumNodes();
	int nBodies = (int) bodies->size();

	// create contact matrix
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
				
				// check any weight for node
				if (contributions->at(j).getWeight() > 0.0 && contactMatrix[nodeId][ibody] == 0) {
					contactMatrix[nodeId][ibody] = 1;
				}
			}
		}
	}
	
	// for each node
	for (int i = 0; i < nNodes; i++) {
		
		// check if any body contributed to node i
		int contactBodyId = -1;
		int contactBodySlaveId = -1;
		
		for (int j = 0; j < nBodies; j++){
			if (contactMatrix[i][j] == 1) 
			{ 
				//verifies if any bodies contributed to node i
				if (contactBodyId >= 0) {
					contactBodySlaveId = j;
				}
				else 
				{
					contactBodyId = j;
				}
			}
		}

		// set contact status
		Node* node = mesh->getNodes()->at(i);
		
		if (contactBodySlaveId >= 0) 
		{
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

// Second contact check
void Contact::secondContactCheck(Mesh* mesh, vector<Body*>* bodies) 
{
	// get number of nodes and bodies 
	int nNodes = (int) mesh->getNumNodes();
	int nBodies = (int) bodies->size();

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
					
						// get closest distance to slave body
						double d = node->getClosestParticleDistanceSlave();
						
						// get normal vector
						Vector3d n = -*node->getUnitNormalTotal();
						

						// particle-node distance
						double dPN = -n.dot(PNVector);

						// check if the distance is the closest
						if (dPN < 0.0) {
							int a = 0; // TODO: remove this line? what is the purpose of this line?
						}
						// check if the distance is the closest
						if (dPN < d && dPN >= 0.0) {
							//set closest distance to slave body
							node->setClosestParticleDistanceSlave(dPN);
							node->PB = particles->at(i)->getId(); // TODO: what is PB?
						}
					}
					else {
						//get closest distance to body
						double d = node->getClosestParticleDistance();

						//get normal vector
						Vector3d n = *node->getUnitNormalTotal();

						//particle-node distance
						double dPN = -n.dot(PNVector);

						if (dPN < 0.0) {
							int a = 0; // TODO: remove this line? what is the purpose of this line?
						}

						if (dPN < d && dPN >= 0.0) {
							//set closest distance to body
							node->setClosestParticleDistance(dPN);
							node->PA = particles->at(i)->getId();
						}
					}
				}
			}
		}
	}

	//for each grid node
	for (int iNode = 0; iNode < nNodes; iNode++) {
		
		if (iNode == 263) { // TODO: node 263 is not being used, remove this line?
			int a = 1;
		}
		Node* node = mesh->getNodes()->at(iNode);

		if (node->getContactStatus()) {
			node->setSecondContactStatus(true);
			
			//for each body
			int contactBodyIdA = node->getContactBodyId(0);
			int contactBodySlaveIdB = node->getContactBodyId(1);

			// get nodal momentum
			Vector3d momentumA = node->getMomentum();
			Vector3d momentumB = *node->getMomentumSlave();

			// get nodal mass
			double massA = node->getMass();
			double massB = node->getMassSlave();

			// get normal vector
			Vector3d n = *node->getUnitNormalTotal();

			// calculate velocity vectors
			Vector3d vA = momentumA / massA;
			Vector3d vB = momentumB / massB;

			// calculate center of mass velocity
			Vector3d vCM = (momentumA + momentumB) / (massA + massB);

			// TODO: what is the purpose of this block of code?
			if (n.dot(massB*momentumA - massA*momentumB) <= 0.0) {

				// TODO thee lines are repeated in the next block of code, can we remove this block?
				node->setSecondContactStatus(false);
				node->setUnitNormalTotal(Vector3d(0, 0, 0));
				node->setNormal(Vector3d(0, 0, 0));
				node->setNormalSlave(Vector3d(0, 0, 0));
			}
			else {
				// contact correction
				double cellDimension = mesh->getCellDimension()[0];
				double contactDistance = node->getClosestParticleDistance() + node->getClosestParticleDistanceSlave();
				if (contactDistance > 0.5 * cellDimension) {
					node->setSecondContactStatus(false);
					node->setUnitNormalTotal(Vector3d(0, 0, 0));
					node->setNormal(Vector3d(0, 0, 0));
					node->setNormalSlave(Vector3d(0, 0, 0));
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

	// get number of nodes and bodies
	int nNodes = (int) mesh->getNumNodes();
	int nBodies = (int) bodies->size();
	
	//for each grid node
	for (int iNode = 0; iNode < nNodes; iNode++) {

		Node* node = mesh->getNodes()->at(iNode);
		
		if (node->getSecondContactStatus()) 
		{
			if (!ModelSetup::getSecondContactActive())
			{
				ModelSetup::setSecondContactActive(true);
			}

			// get contact bodies
			int bodyA = node->getContactBodyId(0);
			int bodyB = node->getContactBodyId(1);

			// TODO: the friction coefficient must be obtained from the material of the particles, not the bodies
			double mu = std::max(bodies->at(bodyB)->getParticles()->at(0)->getMaterial()->getFrictionCoefficient(), bodies->at(bodyA)->getParticles()->at(0)->getMaterial()->getFrictionCoefficient());

			// get nodal mass and momentum
			double massA = node->getMass();
			double massB = node->getMassSlave();
			Vector3d momentumA = node->getMomentum();
			Vector3d momentumB = *node->getMomentumSlave();

			// calculate contact force
			Vector3d f = (massA*momentumB - massB*momentumA) / (massA + massB) / dt;
			// normal vector
			Vector3d n = *node->getUnitNormalTotal();
			
			// calculate normal force
			Vector3d fn = f.dot(n)*n;
			// calculate tangential force
			Vector3d ft = f - fn;
			
			// apply friction
			if (ft.norm() > 0) {
				ft = std::min(ft.norm(), mu * fn.norm()) * ft / ft.norm();
			}

			// set the contact force
			f = ft + fn;
			node->setContactForce(f);

			//add the contact force to the total force
			node->addContactForce();
		}
	}
}

void Contact::setParticlesInContact(Mesh* mesh, vector<Body*>* bodies) {

	// get number of nodes and bodies
	int nNodes = (int) mesh->getNumNodes();
	int nBodies = (int) bodies->size();

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

void Contact::resetParticlesInContact(vector<Body*>* bodies) {

	int nBodies = (int) bodies->size();

	for (size_t ibody = 0; ibody < nBodies; ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
        #pragma omp parallel for shared(particles)
		for (int i = 0; i < particles->size(); ++i) {
			particles->at(i)->setContact(false);
		}
	}
}
