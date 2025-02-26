/*
 * Interpolation.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Interpolation.h"
#include "Contribution.h"
#include "Particle/Particle.h"
#include "Body/Body.h"
#include "Mesh/NodeContact.h"

///
/// From particle to node:
///		
///		These operations have the general form 
///
///			f_I = sum_p N_I(x_p) f_p
///		
///		where,
///
///		f_I: is a nodal quantity
///		f_p: is a particle quantity
///		N_I(x_p): is the weight function of the node I evaluated at particle position x_p
///

void Interpolation::nodalMass(Mesh* mesh, vector<Body*>* bodies) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

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

				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());				
				
				// compute the weighted nodal mass
				const double nodalMass = pMass*contribution->at(j).getWeight();
				
				// check any mass in node
				if (nodalMass<=0.0) { continue; }

				if (nodeI->getId() == 289)
				{
					int a = 1;
				}
				if (nodeI->getId() == 292)
				{
					int a = 1;
				}
		
				// the node is inactivate if he doesn't have mass
				nodeI->setActive(true);

				if (ModelSetup::getContactActive()) {
					if (nodeI->getContactStatus()) {
						//check if the body is set as slave
						if (ibody == nodeI->getContactBodyId(1)) {
							// add mass at node
							nodeI->addMassSlave(nodalMass);
						}
						else {
							// add mass at node
							nodeI->addMass(nodalMass);
						}
					}
					else {
						// add mass at node
						nodeI->addMass(nodalMass);
					}
				}
				else {
					// add mass at node
					nodeI->addMass(nodalMass);
				}
			}
		}
	}
}

void Interpolation::nodalMassFuid(Mesh* mesh, vector<Body*>* bodies) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }
			
			// only saturated particles can interpolate mass of fluid
			if (particles->at(i)->getSaturation()<=0.0) { continue;	}

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// get the particle mass
			const double pMassFluid = particles->at(i)->getMassFluid();

			// for each node in the contribution list 
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// compute the weighted nodal mass
				const double nodalMassFluid = pMassFluid*contribution->at(j).getWeight();
				
				// check any mass in node
				if (nodalMassFluid<=0.0) { continue; }
		
				// the node is inactivate if he doesn't have mass
				nodeI->setActive(true);

				// add mass at node
				nodeI->addMassFluid(nodalMassFluid);
			}
		}
	}
}

void Interpolation::nodalMomentum(Mesh* mesh, vector<Body*>* bodies) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

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

			// get particle velocity
			const Vector3d pVelocity = particles->at(i)->getVelocity();

			// get particle mass
			const double pMass = particles->at(i)->getMass();
			
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				if (ModelSetup::getContactActive()) {
					if (nodeI->getContactStatus()) {
						//check if the body is set as slave
						if (ibody == nodeI->getContactBodyId(1)) {
							// add the weighted momentum in node
							nodeI->addMomentumSlave(pMass * pVelocity * contribution->at(j).getWeight());
						}
						else {
							// add the weighted momentum in node
							nodeI->addMomentum(pMass * pVelocity * contribution->at(j).getWeight());
						}
					}
					else {
						// add the weighted momentum in node
						nodeI->addMomentum(pMass * pVelocity * contribution->at(j).getWeight());
					}
				}
				else {
					// add the weighted momentum in node
					nodeI->addMomentum(pMass * pVelocity * contribution->at(j).getWeight());
				}
			}
		}
	}
}

void Interpolation::nodalMomentumFluid(Mesh* mesh, vector<Body*>* bodies) {

	// check if is two-phase calculations
	if(!ModelSetup::getTwoPhaseActive()) return;

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// only saturated particles can interpolate mass of fluid
			if (particles->at(i)->getSaturation()<=0.0) { continue;	}

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// get particle velocity of fluid
			const Vector3d pVelocityFluid = *(particles->at(i)->getVelocityFluid());

			// get particle mass
			const double pMassFluid = particles->at(i)->getMassFluid();
			
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// add the weighted momentum in node
				nodeI->addMomentumFluid(pMassFluid*pVelocityFluid*contribution->at(j).getWeight());
			}
		}
	}
}

void Interpolation::nodalUnitNormal(Mesh* mesh, vector<Body*>* bodies) {
	
	int nNodes = mesh->getNumNodes();

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

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

				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// compute the nodal mass gradient
				const Vector3d nodalMassGradient = pMass * contribution->at(j).getGradients();

				if (ModelSetup::getContactActive()) {
					if (nodeI->getContactStatus()) {
						//check if the body is set as slave
						if (ibody == nodeI->getContactBodyId(1)) {
							// add mass at node
							nodeI->addMassGradientSlave(nodalMassGradient);
						}
						else {
							// add mass at node
							nodeI->addMassGradient(nodalMassGradient);
						}
					}
					//else {
					//	// add mass at node
					//	nodeI->addMassGradient(nodalMassGradient);
					//}
				}
				//else {
				//	// add mass at node
				//	nodeI->addMassGradient(nodalMassGradient);
				//}
			}
		}
	}
	
	//for each grid node
	for (int iNode = 0; iNode < nNodes; iNode++) {
		Node* node = nodes->at(iNode);

		if (node->getContactStatus()) {
			// nodal normal vector master
			Vector3d nA = node->getNormal()->normalized();
			
			// nodal normal vector slave
			Vector3d nB = node->getNormalSlave()->normalized();

			// nodal unit normal vector
			Vector3d n = (nA - nB).normalized();

			/*if (iNode == 353) {
				if (n[0] < 0.57878) {

					if (n[0] > 0.57876) {
						int a = 1;
					}
				}
			}*/
			
			//node->setUnitNormalTotal(n);
			//node->setUnitNormalTotal(Vector3d (0.0, 0.0, 1.0));
			node->setUnitNormalTotal(nA);
		}
	}
}

void Interpolation::nodalInternalForce(Mesh* mesh, vector<Body*>* bodies) {

	// is two-phase calculations
	bool isTwoPhase = ModelSetup::getTwoPhaseActive();

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

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

			// get effective stress of solid
			const Matrix3d pStress = particles->at(i)->getStress();
			
			// get the particle volume
			double pVolume = particles->at(i)->getCurrentVolume();

			// particle pressure
			double pPressure{0};
			
			if (isTwoPhase && particles->at(i)->getSaturation()>0.0) {

				// get pore pressure pressure
				pPressure = particles->at(i)->getPressureFluid();
			}

			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradients
				const Vector3d gradient = contribution->at(j).getGradients();

				// compute the particle's contribution to the nodal internal force
				Vector3d internalForce;
				internalForce.x()=-(pStress(0,0)*gradient(0)+pStress(1,0)*gradient(1)+pStress(2,0)*gradient(2))*pVolume;
				internalForce.y()=-(pStress(0,1)*gradient(0)+pStress(1,1)*gradient(1)+pStress(2,1)*gradient(2))*pVolume;
				internalForce.z()=-(pStress(0,2)*gradient(0)+pStress(1,2)*gradient(1)+pStress(2,2)*gradient(2))*pVolume;

				if (isTwoPhase && particles->at(i)->getSaturation()>0.0)
				{
					internalForce.x()+=pPressure*gradient(0)*pVolume;
					internalForce.y()+=pPressure*gradient(1)*pVolume;
					internalForce.z()+=pPressure*gradient(2)*pVolume;
				}

				if (ModelSetup::getContactActive()) {
					if (nodeI->getContactStatus()) {
						//check if the body is set as slave
						if (ibody == nodeI->getContactBodyId(1)) {
							// add the internal force contribution in slave node
							nodeI->addInternalForceSlave(internalForce);
						}
						else {
							// add the internal force contribution in node
							nodeI->addInternalForce(internalForce);
						}
					}
					else {
						// add the internal force contribution in node
						nodeI->addInternalForce(internalForce);
					}
				}
				else {
					// add the internal force contribution in node
					nodeI->addInternalForce(internalForce);
				}
			}
		}
	}
}

void Interpolation::nodalInternalForceFluid(Mesh* mesh, vector<Body*>* bodies) {

	// check if is two-phase calculations
	if(!ModelSetup::getTwoPhaseActive()) return;

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// only saturated particles can interpolate mass of fluid
			if (particles->at(i)->getSaturation()<=0.0) { continue;	}

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// get particle pressure
			const double pPressure = particles->at(i)->getPressureFluid();
			
			// get current particle volume
			const double pVolume = particles->at(i)->getCurrentVolume();

			// get current porosity
			const double porosity = particles->at(i)->getPorosity();
			
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradients
				const Vector3d gradient = contribution->at(j).getGradients();

				// compute the particle's contribution to the nodal internal force
				Vector3d internalForceFluid;
				internalForceFluid.x()=pPressure*gradient(0)*pVolume*porosity;
				internalForceFluid.y()=pPressure*gradient(1)*pVolume*porosity;
				internalForceFluid.z()=pPressure*gradient(2)*pVolume*porosity;

				// add the internal of fluid force contribution in node
				nodeI->addInternalForceFluid(internalForceFluid);
			}
		}
	}
}

void Interpolation::nodalExternalForce(Mesh* mesh, vector<Body*>* bodies) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

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

			// get particle external force
			const Vector3d pExtForce = particles->at(i)->getExternalForce();

			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				if (ModelSetup::getContactActive()) {
					if (nodeI->getContactStatus()) {
						//check if the body is set as slave
						if (ibody == nodeI->getContactBodyId(1)) {
							// add the internal force contribution in slave node
							nodeI->addExternalForceSlave(pExtForce * contribution->at(j).getWeight());
						}
						else {
							// add weighted force in node
							nodeI->addExternalForce(pExtForce * contribution->at(j).getWeight());
						}
					}
					else {
						// add weighted force in node
						nodeI->addExternalForce(pExtForce * contribution->at(j).getWeight());
					}
				}
				else {
					// add weighted force in node
					nodeI->addExternalForce(pExtForce * contribution->at(j).getWeight());
				}
			}
		}
	}
}

void Interpolation::nodalExternalForceFluid(Mesh* mesh, vector<Body*>* bodies) {

	// check if is two-phase calculations
	if(!ModelSetup::getTwoPhaseActive()) return;
	
	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// only saturated particles can interpolate mass of fluid
			if (particles->at(i)->getSaturation()<=0.0) { continue;	}

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// get particle external force
			const Vector3d pExtForceFluid = *(particles->at(i)->getExternalForceFluid());

			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// add weighted force in node
				nodeI->addExternalForceFluid(pExtForceFluid*contribution->at(j).getWeight());
			}
		}
	}
}

void Interpolation::nodalDragForceFluid(Mesh* mesh, vector<Body*>* bodies) {

	// check if is two-phase calculations
	if(!ModelSetup::getTwoPhaseActive()) return;
	
	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// only saturated particles can interpolate mass of fluid
			if (particles->at(i)->getSaturation()<=0.0) { continue;	}

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// get particle drag force 
			const Vector3d pDragForceFluid = particles->at(i)->getDragForceFluid();

			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// add weighted force in node
				nodeI->addExternalForceFluid(pDragForceFluid*contribution->at(j).getWeight());
			}
		}
	}
}

///
/// From node to particle:
///		
///		These operations have the general form 
///
///			f_p = sum_I N_I(x_p) f_I
///		
///		where,
///
///		f_p: is a particle quantity
///		f_I: is a nodal quantity
///		N_I(x_p): is the weight function of the node I evaluated at particle position x_p
///

void Interpolation::particleStrainIncrement(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

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

			// initialize a matrix for strain increment computation
			Matrix3d dstrain = Matrix3d::Zero();
		
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {
				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradient
				const Vector3d dN = contribution->at(j).getGradients();

				//initialize vector v
				Vector3d v = Vector3d::Zero();

				if (ModelSetup::getContactActive()) {
					if (nodeI->getContactStatus()) {
						//check if the body is set as slave
						if (ibody == nodeI->getContactBodyId(1)) {
							// get nodal velocity
							v = *nodeI->getVelocitySlave();
						}
						else {
							// get nodal velocity
							v = nodeI->getVelocity();
						}
					}
					else {
						// get nodal velocity
						v = nodeI->getVelocity();
					}
				}
				else {
					// get nodal velocity
					v = nodeI->getVelocity();
				}
				
				// compute the nodal contribution to the particle strain increment
				dstrain(0,0) += (dN(0)*v(0)+dN(0)*v(0))*0.5*dt; // x,x
				dstrain(0,1) += (dN(1)*v(0)+dN(0)*v(1))*0.5*dt; // x,y
				dstrain(0,2) += (dN(2)*v(0)+dN(0)*v(2))*0.5*dt; // x,z

				dstrain(1,0) += (dN(0)*v(1)+dN(1)*v(0))*0.5*dt; // y,x
				dstrain(1,1) += (dN(1)*v(1)+dN(1)*v(1))*0.5*dt; // y,y
				dstrain(1,2) += (dN(2)*v(1)+dN(1)*v(2))*0.5*dt; // y,z
				
				dstrain(2,0) += (dN(0)*v(2)+dN(2)*v(0))*0.5*dt; // z,x
				dstrain(2,1) += (dN(1)*v(2)+dN(2)*v(1))*0.5*dt; // z,y
				dstrain(2,2) += (dN(2)*v(2)+dN(2)*v(2))*0.5*dt; // z,z
			}
			
			// set total particle strain increment
			particles->at(i)->setStrainIncrement(dstrain);
		}
	}
}

void Interpolation::particleStrainIncrementFluid(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// only saturated particles can interpolate mass of fluid
			if (particles->at(i)->getSaturation()<=0.0) { continue;	}

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// initialize a matrix for strain increment computation
			Matrix3d dstrain = Matrix3d::Zero();
			
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradient
				const Vector3d dN = contribution->at(j).getGradients();

				// get nodal velocity
				const Vector3d v = *(nodeI->getVelocityFluid());

				// compute the nodal contribution to the particle strain increment

				dstrain(0,0) += (dN(0)*v(0)+dN(0)*v(0))*0.5*dt; // x,x
				dstrain(0,1) += (dN(1)*v(0)+dN(0)*v(1))*0.5*dt; // x,y
				dstrain(0,2) += (dN(2)*v(0)+dN(0)*v(2))*0.5*dt; // x,z

				dstrain(1,0) += (dN(0)*v(1)+dN(1)*v(0))*0.5*dt; // y,x
				dstrain(1,1) += (dN(1)*v(1)+dN(1)*v(1))*0.5*dt; // y,y
				dstrain(1,2) += (dN(2)*v(1)+dN(1)*v(2))*0.5*dt; // y,z
				
				dstrain(2,0) += (dN(0)*v(2)+dN(2)*v(0))*0.5*dt; // z,x
				dstrain(2,1) += (dN(1)*v(2)+dN(2)*v(1))*0.5*dt; // z,y
				dstrain(2,2) += (dN(2)*v(2)+dN(2)*v(2))*0.5*dt; // z,z
			}

			// set total particle strain increment
			particles->at(i)->setStrainIncrementFluid(dstrain);
		}
	}
}

void Interpolation::particleVorticityIncrement(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle 
		for (size_t i = 0; i < particles->size(); ++i) {

			// only active particles can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// get nodes and weights that the particle contributes
			const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

			// initialize a matrix for spin increment computation
			Matrix3d dvorticity = Matrix3d::Zero();
			
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// get nodal gradient
				const Vector3d dN = contribution->at(j).getGradients();

				//initialize vector v
				Vector3d v = Vector3d::Zero();

				if (ModelSetup::getContactActive()) {
					if (nodeI->getContactStatus()) {
						//check if the body is set as slave
						if (ibody == nodeI->getContactBodyId(1)) {
							// get nodal velocity
							v = *nodeI->getVelocitySlave();
						}
						else {
							// get nodal velocity
							v = nodeI->getVelocity();
						}
					}
					else {
						// get nodal velocity
						v = nodeI->getVelocity();
					}
				}
				else {
					// get nodal velocity
					v = nodeI->getVelocity();
				}
				
				// compute the nodal contribution to the particle spin increment

				dvorticity(0,0) += (dN(0)*v(0)-dN(0)*v(0))*0.5*dt; // x,x
				dvorticity(0,1) += (dN(1)*v(0)-dN(0)*v(1))*0.5*dt; // x,y
				dvorticity(0,2) += (dN(2)*v(0)-dN(0)*v(2))*0.5*dt; // x,z
				
				dvorticity(1,0) += (dN(0)*v(1)-dN(1)*v(0))*0.5*dt; // y,x
				dvorticity(1,1) += (dN(1)*v(1)-dN(1)*v(1))*0.5*dt; // y,y
				dvorticity(1,2) += (dN(2)*v(1)-dN(1)*v(2))*0.5*dt; // y,z
				
				dvorticity(2,0) += (dN(0)*v(2)-dN(2)*v(0))*0.5*dt; // z,x
				dvorticity(2,1) += (dN(1)*v(2)-dN(2)*v(1))*0.5*dt; // z,y
				dvorticity(2,2) += (dN(2)*v(2)-dN(2)*v(2))*0.5*dt; // z,z
			}

			// add total spin tensor in the particle
			particles->at(i)->setVorticityIncrement(dvorticity);
		}
	}
}

void Interpolation::particleDeformationGradient(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

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

			// initialize a matrix for velocity gradient
			Matrix3d gradV = Matrix3d::Zero();
			
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get the contributing node
				Node* nodeI = nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradient
				const Vector3d dN = contribution->at(j).getGradients();

				// get nodal velocity
				const Vector3d v = nodeI->getVelocity();

				// compute the nodal contribution deformation gradient increment

				gradV(0,0) += (dN(0)*v(0)); // x,x
				gradV(0,1) += (dN(1)*v(0)); // x,y
				gradV(0,2) += (dN(2)*v(0)); // x,z

				gradV(1,0) += (dN(0)*v(1)); // y,x
				gradV(1,1) += (dN(1)*v(1)); // y,y
				gradV(1,2) += (dN(2)*v(1)); // y,z
				
				gradV(2,0) += (dN(0)*v(2)); // z,x
				gradV(2,1) += (dN(1)*v(2)); // z,y
				gradV(2,2) += (dN(2)*v(2)); // z,z
			}

			// get deformation gradient
			const Matrix3d Fn = particles->at(i)->getDeformationGradient();

			// set current deformation gradient
			particles->at(i)->setDeformationGradient((Matrix3d::Identity()+dt*gradV)*Fn);
		}
	}
}

// Funci�n para interpolar valores de tipo Eigen::Vector3d en el tiempo itime
Eigen::Vector3d Interpolation::interpolateVector(const std::vector<double>& times, const std::vector<Eigen::Vector3d>& values, double itime) {
	
	if (itime <= times.front()) return values.front();
	if (itime >= times.back()) return values.back();

	// Encontrar el indice del tiempo inmediatamente superior a itime
	auto upper = std::upper_bound(times.begin(), times.end(), itime);
	size_t idx = std::distance(times.begin(), upper) - 1;

	// Valores adyacentes para la interpolaci�n
	double t0 = times[idx];
	double t1 = times[idx + 1];
	Eigen::Vector3d v0 = values[idx];
	Eigen::Vector3d v1 = values[idx + 1];

	// Interpolacion lineal para cada componente
	double factor = (itime - t0) / (t1 - t0);
	Eigen::Vector3d interpolatedValue = v0 + factor * (v1 - v0);

	return interpolatedValue;
}
