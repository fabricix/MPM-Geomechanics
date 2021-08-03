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
	vector<Node>* nodes = mesh->getNodes();

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
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// compute the weighted nodal mass
				const double nodalMass = pMass*contribution->at(j).getWeight();
				
				// check any mass in node
				if (nodalMass<=0.0) { continue; }
		
				// the node is inactivate if he doesn't have mass
				nodeI->setActive(true);

				// add mass at node
				nodeI->addMass(nodalMass);
			}
		}
	}
}

void Interpolation::nodalMassFuid(Mesh* mesh, vector<Body*>* bodies) {

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

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
			const double pMassFluid = particles->at(i)->getMassFluid();

			// for each node in the contribution list 
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get the contributing node
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

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
	vector<Node>* nodes = mesh->getNodes();

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
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// add the weighted momentum in node
				nodeI->addMomentum(pMass*pVelocity*contribution->at(j).getWeight());
			}
		}
	}
}

void Interpolation::nodalMomentumFluid(Mesh* mesh, vector<Body*>* bodies) {

	// check if is two-phase calculations
	if(!ModelSetup::getTwoPhaseActive()) return;

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

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
			const Vector3d pVelocityFluid = *(particles->at(i)->getVelocityFluid());

			// get particle mass
			const double pMassFluid = particles->at(i)->getMassFluid();
			
			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get the contributing node
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// add the weighted momentum in node
				nodeI->addMomentumFluid(pMassFluid*pVelocityFluid*contribution->at(j).getWeight());
			}
		}
	}
}

void Interpolation::nodalInternalForce(Mesh* mesh, vector<Body*>* bodies) {

	// is two-phase calculations
	bool isTwoPhase = ModelSetup::getTwoPhaseActive();

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

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

			// get particle stress
			const Matrix3d pStress = particles->at(i)->getStress();
			
			// get the particle volume
			double pVolume = particles->at(i)->getMass()/particles->at(i)->getDensity();

			// particle pressure
			Matrix3d pPressure;
			
			if (isTwoPhase) {

				// get pore pressure pressure
				pPressure = particles->at(i)->getPressureFluid()*Matrix3d::Identity();
				
				// in two phase calculation the volume is the total volume, Vt = Vs / (1-n)
				pVolume /= (1.0-particles->at(i)->getPorosity());
			}

			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradients
				const Vector3d gradient = contribution->at(j).getGradients();

				// compute the particle's contribution to the nodal internal force
				Vector3d internalForce;
				internalForce.x()=-(pStress(0,0)*gradient(0)+pStress(1,0)*gradient(1)+pStress(2,0)*gradient(2))*pVolume;
				internalForce.y()=-(pStress(0,1)*gradient(0)+pStress(1,1)*gradient(1)+pStress(2,1)*gradient(2))*pVolume;
				internalForce.z()=-(pStress(0,2)*gradient(0)+pStress(1,2)*gradient(1)+pStress(2,2)*gradient(2))*pVolume;

				if (isTwoPhase)
				{
					internalForce.x()+=(pPressure(0,0)*gradient(0)+pPressure(1,0)*gradient(1)+pPressure(2,0)*gradient(2))*pVolume;
					internalForce.y()+=(pPressure(0,1)*gradient(0)+pPressure(1,1)*gradient(1)+pPressure(2,1)*gradient(2))*pVolume;
					internalForce.z()+=(pPressure(0,2)*gradient(0)+pPressure(1,2)*gradient(1)+pPressure(2,2)*gradient(2))*pVolume;
				}

				// add the internal force contribution in node
				nodeI->addInternalForce(internalForce);
			}
		}
	}
}

void Interpolation::nodalInternalForceFluid(Mesh* mesh, vector<Body*>* bodies) {

	// check if is two-phase calculations
	if(!ModelSetup::getTwoPhaseActive()) return;

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

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

			// get particle stress
			const Matrix3d pPressure = particles->at(i)->getPressureFluid()*Matrix3d::Identity();
			
			// get the particle volume
			const double pVolumeFluid = particles->at(i)->getMassFluid()/particles->at(i)->getDensityFluid();

			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradients
				const Vector3d gradient = contribution->at(j).getGradients();

				// compute the particle's contribution to the nodal internal force
				Vector3d internalForceFluid;
				internalForceFluid.x()=(pPressure(0,0)*gradient(0)+pPressure(1,0)*gradient(1)+pPressure(2,0)*gradient(2))*pVolumeFluid;
				internalForceFluid.y()=(pPressure(0,1)*gradient(0)+pPressure(1,1)*gradient(1)+pPressure(2,1)*gradient(2))*pVolumeFluid;
				internalForceFluid.z()=(pPressure(0,2)*gradient(0)+pPressure(1,2)*gradient(1)+pPressure(2,2)*gradient(2))*pVolumeFluid;

				// add the internal of fluid force contribution in node
				nodeI->addInternalForceFluid(internalForceFluid);
			}
		}
	}
}

void Interpolation::nodalExternalForce(Mesh* mesh, vector<Body*>* bodies) {

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

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
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// add weighted force in node
				nodeI->addExternalForce(pExtForce*contribution->at(j).getWeight());
			}
		}
	}
}

void Interpolation::nodalExternalForceFluid(Mesh* mesh, vector<Body*>* bodies) {

	// check if is two-phase calculations
	if(!ModelSetup::getTwoPhaseActive()) return;
	
	// get nodes
	vector<Node>* nodes = mesh->getNodes();

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
			const Vector3d pExtForceFluid = *(particles->at(i)->getExternalForceFluid());

			// get particle drag force 
			const Vector3d pDragForceFluid = particles->at(i)->getDragForceFluid();

			// for each node in the contribution list
			for (size_t j = 0; j < contribution->size(); ++j) {

				// get contributing node
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// add weighted force in node
				nodeI->addExternalForceFluid((pExtForceFluid+pDragForceFluid)*contribution->at(j).getWeight());
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
	vector<Node>* nodes = mesh->getNodes();

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
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// get the nodal gradient
				const Vector3d dN = contribution->at(j).getGradients();

				// get nodal velocity
				const Vector3d v = nodeI->getVelocity();

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

void Interpolation::particleVorticityIncrement(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

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
				Node* nodeI = &nodes->at(contribution->at(j).getNodeId());

				// get nodal gradient
				const Vector3d dN = contribution->at(j).getGradients();

				// get nodal velocity
				const Vector3d v = nodeI->getVelocity();

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
