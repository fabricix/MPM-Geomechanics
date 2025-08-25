// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "ContactManager.h"



void ContactManager::contactCheck(Mesh* mesh, vector<Body*>* bodies) {
	ModelSetup::setContactActive(false);
	mesh->clearContactNodes();

	// get number of nodes and bodies
	size_t nNodes = (int)mesh->getNumNodes();
	size_t nBodies = (int)bodies->size();

	// create contribution matrix - Example:
	//			body1	body2	
	// node1	1		0		-> indicates that node1 receives contribution from body1
	// node2    0		1		-> indicates that node2 receives contribution from body2
	// node3    1		1		-> indicates that node3 receives contribution from body1 and body2
	// node4    0		0		-> indicates that node4 does not receive contribution any body
	
	vector<vector<int>> contributionMatrix(static_cast<int>(nNodes), vector<int>(nBodies, 0));

	// for each body
	for (size_t ibody = 0; ibody < nBodies; ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// get the contribution nodes
			vector<Contribution>* contributions = particles->at(i)->getContributionNodes();

			for (size_t j = 0; j < contributions->size(); j++) {

				int nodeId = contributions->at(j).getNodeId();

				// check any weight for node
				if (contributions->at(j).getWeight() > 0.0 && contributionMatrix[nodeId][ibody] == 0) {
					contributionMatrix[nodeId][ibody] = 1;
				}
			}
		}
	}

	
	// for each node
	for (size_t i = 0; i < nNodes; i++) {

		// check if any body contributed to node i
		Mesh::ContactNodeData Contact = Mesh::ContactNodeData();
		
		Contact.nodeId = i;

		// check if any bodies j contributed to node i
		for (size_t j = 0; j < nBodies; j++) {
			if (contributionMatrix[i][j] == 1)
			{
				// check if the the body i received contribution from another body
				if (Contact.bodyMasterId >= 0) {
					Contact.bodySlaveId = j;
				}
				else
				{
					Contact.bodyMasterId = j;
				}
			}
		}

		// set contact status
		if (Contact.bodySlaveId >= 0)
		{
			contactNodes[i] = Contact;
			ModelSetup::setContactActive(true);
		}
	}
	if (ModelSetup::getContactActive()) {
		mesh->setContactNodes(contactNodes);
	}
}

void ContactManager::contactCheckCorrection(Mesh* mesh, vector<Body*>* bodies) {
	// get number of nodes and bodies 
	size_t nBodies = (int)bodies->size();

	unordered_map<int, Mesh::ContactNodeData>& contactNodes = mesh->getContactNodes();

	//Calculation of the distance between bodies
	//for each body
	for (size_t ibody = 0; ibody < nBodies; ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			Vector3d pPosition = particles->at(i)->getPosition();

			// get the contribution nodes
			vector<Contribution>* contributions = particles->at(i)->getContributionNodes();

			for (size_t j = 0; j < contributions->size(); j++) {

				int nodeId = contributions->at(j).getNodeId();
				Node* node = mesh->getNodes()->at(nodeId);

				// check any weight for node
				if (contributions->at(j).getWeight() <= 0.0) { continue; }

				Vector3d nCoordinates = node->getCoordinates();

				// Particle-node vector
				Vector3d PNVector = pPosition - nCoordinates;

				//check contact at this node
				auto it = contactNodes.find(contributions->at(j).getNodeId());

				if (it != contactNodes.end()) {
					Mesh::ContactNodeData& contactNodeData = it->second; 

					//add mass at node of the master body 
					if (static_cast<int>(ibody) == contactNodeData.bodyMasterId) {
						//get closest distance to master body
						double d = contactNodeData.closestParticleDistanceMaster;

						//get normal vector
						Vector3d n = contactNodeData.normal;

						//particle-node distance
						double dPN = -n.dot(PNVector);

						if (dPN < d && dPN >= 0.0) {
							//set closest distance to body
							contactNodeData.closestParticleDistanceMaster = dPN;
						}
					}
					//add mass at node of the slave body 
					else {
						// get closest distance to slave body
						double d = contactNodeData.closestParticleDistanceSlave;

						// get normal vector
						Vector3d n = -contactNodeData.normal;


						// particle-node distance
						double dPN = -n.dot(PNVector);

						// check if the distance is the closest
						if (dPN < d && dPN >= 0.0) {
							//set closest distance to slave body
							contactNodeData.closestParticleDistanceSlave = dPN;
						}
					}
				}
			}
		}
	}


	//for each contact node
	for (auto it = contactNodes.begin(); it != contactNodes.end();) {
		Mesh::ContactNodeData& contactNodesData = it->second;

		// get nodal momentum
		Vector3d momentumA = contactNodesData.momentumMaster;
		Vector3d momentumB = contactNodesData.momentumSlave;

		// get nodal mass
		double massA = contactNodesData.massMaster;
		double massB = contactNodesData.massSlave;

		// get normal vector
		Vector3d n = contactNodesData.normal;

		if (n.dot(massB * momentumA - massA * momentumB) <= 0.0) {
			it = contactNodes.erase(it); 
		}
		else {
			// contact correction
			double cellDimension = mesh->getCellDimension()[0];
			double contactDistance = contactNodesData.closestParticleDistanceMaster + contactNodesData.closestParticleDistanceSlave;
			if (contactDistance > 0.5 * cellDimension) {
				it = contactNodes.erase(it);
			}
			else {
				++it;
			}
		}
	}
}

void ContactManager::nodalUnitNormal(Mesh* mesh, vector<Body*>* bodies) {

	unordered_map<int, Mesh::ContactNodeData>& contactNodes = mesh->getContactNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// get the particle mass
			const double pMass = particles->at(i)->getMass();

			// for each node in the contribution list 
			for (size_t j = 0; j < contribution->size(); ++j) {

				// compute the nodal mass gradient
				const Vector3d nodalMassGradient = pMass * contribution->at(j).getGradients();


				//check contact at this node
				auto it = contactNodes.find(contribution->at(j).getNodeId());

				if (it != contactNodes.end()) {
					Mesh::ContactNodeData& contactNodeData = it->second;

					//add mass at node of the master body 
					if (static_cast<int>(ibody) == contactNodeData.bodyMasterId) {
						contactNodeData.normalMaster += nodalMassGradient;
					}
					//add mass at node of the slave body 
					else {
						contactNodeData.normalSlave += nodalMassGradient;
					}
				}
			}
		}
	}

	//for each contact node
	for (auto it = contactNodes.begin(); it != contactNodes.end(); ++it) {
		Mesh::ContactNodeData& contactNodesData = it->second;

		// nodal normal vector master
		Vector3d nM = contactNodesData.normalMaster.normalized();

		// nodal normal vector slave
		Vector3d nS = contactNodesData.normalSlave.normalized();

		// nodal unit normal vector
		Vector3d n = (nM - nS).normalized();

		contactNodesData.normal = n;
	}
}

void ContactManager::computeContactForces(Mesh* mesh, vector<Body*>* bodies, double dt) {

	contactCheckCorrection(mesh, bodies);

	unordered_map<int, Mesh::ContactNodeData>& contactNodes = mesh->getContactNodes();


	if (contactNodes.size() == 0) {
		ModelSetup::setContactActive(false);
	}

	for (auto it = contactNodes.begin(); it != contactNodes.end(); ++it) {
		Mesh::ContactNodeData& contactNodesData = it->second;

		// get contact bodies
		int bodyA = contactNodesData.bodyMasterId;
		int bodyB = contactNodesData.bodySlaveId;

		double muA = bodies->at(bodyA)->getParticles()->at(0)->getMaterial()->getFrictionCoefficient();
		double muB = bodies->at(bodyB)->getParticles()->at(0)->getMaterial()->getFrictionCoefficient();

		// get friction coefficient
		double mu = std::min(muA, muB);

		// get nodal mass and momentum
		double massA = contactNodesData.massMaster;
		double massB = contactNodesData.massSlave;
		Vector3d momentumA = contactNodesData.momentumMaster;
		Vector3d momentumB = contactNodesData.momentumSlave;

		// calculate contact force
		Vector3d f = (massA * momentumB - massB * momentumA) / (massA + massB) / dt;
		// normal vector
		Vector3d n = contactNodesData.normal;

		// calculate normal force
		Vector3d fn = f.dot(n) * n;
		// calculate tangential force
		Vector3d ft = f - fn;

		// apply friction
		if (ft.norm() > 0) {
			ft = std::min(ft.norm(), mu * fn.norm()) * ft / ft.norm();
		}

		// set the contact force
		f = ft + fn;

		contactNodesData.contactForce = f;
		contactNodesData.normalContactForce = fn;
		contactNodesData.tangentialContactForce = ft;

		//add the contact force to the total force
		contactNodesData.totalForceMaster += f;
		contactNodesData.totalForceSlave -= f;
	}
}