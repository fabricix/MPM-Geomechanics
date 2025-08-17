// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Interpolation.h"
#include "Contribution.h"
#include "Particle/Particle.h"
#include "Body/Body.h"
#include "Loads.h"
#include "TerrainContact.h"
#include "Seismic.h"

#include <omp.h>

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

void Interpolation::nodalMass(Mesh* mesh, vector<Particle*>* particles) 
{
	// get nodes
	vector<Node*>* nodes = mesh->getNodes();
	const int nNodes = static_cast<int>(nodes->size());

	#pragma omp parallel for schedule(static)
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
			if (nodalMass<=0.0)  continue;

			#pragma omp atomic
			nodeI->getMassRef() += nodalMass;
		}
	}

	// Node activation: mark nodes with mass > 0 as active
	#pragma omp parallel for
    for (int i = 0; i < nNodes; ++i) {
        if (nodes->at(i)->getMass() > 0.0)
            nodes->at(i)->setActive(true);
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

void Interpolation::nodalMomentum(Mesh* mesh, vector<Particle*>* particles) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();
	
	#pragma omp parallel for schedule(static)
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
			
			// nodal momentum
			const Vector3d nodalMomentum = pMass*pVelocity*contribution->at(j).getWeight();

			// add the weighted momentum in node
			#pragma omp atomic update
			nodeI->getMomentumRef().x() += nodalMomentum.x();

			#pragma omp atomic update
			nodeI->getMomentumRef().y() += nodalMomentum.y();

			#pragma omp atomic update
			nodeI->getMomentumRef().z() += nodalMomentum.z();
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

void Interpolation::nodalInternalForce(Mesh* mesh, vector<Particle*>* particles) {
	
	// is two-phase calculations
	bool isTwoPhase = ModelSetup::getTwoPhaseActive();
	
	// check if is one direction hydro-mechanical coupling
	bool isOneDirectionHydromechanicalCoupling = ModelSetup::getHydroMechOneWayEnabled();
	
	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	#pragma omp parallel for schedule(static)
	for (size_t i = 0; i < particles->size(); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }

		// get nodes and weights that the particle contributes
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// get effective stress of solid
		Matrix3d pStress = particles->at(i)->getStress();

		// use total stress if hydro-mechanical coupling is enabled
		if (isOneDirectionHydromechanicalCoupling) {
			pStress -= particles->at(i)->getPorePressure() * Matrix3d::Identity();
		}

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

			// add the internal force contribution in node
			#pragma omp atomic update
			nodeI->getInternalForceRef().x() += internalForce.x();

			#pragma omp atomic update
			nodeI->getInternalForceRef().y() += internalForce.y();

			#pragma omp atomic update
			nodeI->getInternalForceRef().z() += internalForce.z();
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

void Interpolation::nodalExternalForce(Mesh* mesh, vector<Particle*>* particles) {

	// The nodal external force is calculated from two sources,
	// one from the stored force in the particles (1), like gravity,
	// and two, from the external force imposed directly at the grid nodes (2)
	
	// (1) - External force from particles: f_ext_I = sum_p f_ext_p N_Ip
	vector<Node*>* nodes = mesh->getNodes();

	#pragma omp parallel for schedule(static)
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

			// add weighted force in node
			const Vector3d externalForce = pExtForce*contribution->at(j).getWeight();
			
			// add the external force contribution in node
			#pragma omp atomic update
			nodeI->getExternalForceRef().x() += externalForce.x();

			#pragma omp atomic update
			nodeI->getExternalForceRef().y() += externalForce.y();

			#pragma omp atomic update
			nodeI->getExternalForceRef().z() += externalForce.z();
		}
	}

	// (2) - External force from nodes: f_ext_I += f_ext_BC
	// External boundary condition force 
	Loads::NodalPointLoadData& nodal_force_list = Loads::getNodalPointList();
	for (size_t i = 0; i < nodal_force_list.loads.size() ; i++)
	{
		int node_id = nodal_force_list.nodal_ids.at(i);
		Vector3d extload = nodal_force_list.loads.at(i);
		mesh->getNodes()->at(node_id)->addExternalForce(extload);
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

void Interpolation::particleStrainIncrement(Mesh* mesh, vector<Particle*>* particles, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	// for each particle
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

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

			// get nodal velocity
			const Vector3d v = nodeI->getVelocity();

			// compute the nodal contribution to the particle strain increment

			dstrain(0,0) += (dN(0)*v(0) + dN(0)*v(0)) * 0.5 * dt; // x,x
			dstrain(0,1) += (dN(1)*v(0) + dN(0)*v(1)) * 0.5 * dt; // x,y
			dstrain(0,2) += (dN(2)*v(0) + dN(0)*v(2)) * 0.5 * dt; // x,z

			dstrain(1,0) += (dN(0)*v(1) + dN(1)*v(0)) * 0.5 * dt; // y,x
			dstrain(1,1) += (dN(1)*v(1) + dN(1)*v(1)) * 0.5 * dt; // y,y
			dstrain(1,2) += (dN(2)*v(1) + dN(1)*v(2)) * 0.5 * dt; // y,z
			
			dstrain(2,0) += (dN(0)*v(2) + dN(2)*v(0)) * 0.5 * dt; // z,x
			dstrain(2,1) += (dN(1)*v(2) + dN(2)*v(1)) * 0.5 * dt; // z,y
			dstrain(2,2) += (dN(2)*v(2) + dN(2)*v(2)) * 0.5 * dt; // z,z
		}

		// set total particle strain increment
		particles->at(i)->setStrainIncrement(dstrain);
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

void Interpolation::particleVorticityIncrement(Mesh* mesh, vector<Particle*>* particles, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

#ifdef _OPENMP
    #pragma omp parallel for
#endif
    for (int i = 0; i < static_cast<int>(particles->size()); ++i) 
	{
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

			// get nodal velocity
			const Vector3d v = nodeI->getVelocity();

			// compute the nodal contribution to the particle spin increment
            dvorticity(0,1) += (dN(1)*v(0) - dN(0)*v(1)) * 0.5 * dt; // x,y
            dvorticity(0,2) += (dN(2)*v(0) - dN(0)*v(2)) * 0.5 * dt; // x,z
            dvorticity(1,2) += (dN(2)*v(1) - dN(1)*v(2)) * 0.5 * dt; // y,z
		}
		
		// fill the anti-symmetric part of the spin tensor
		dvorticity(1,0) = -dvorticity(0,1);
        dvorticity(2,0) = -dvorticity(0,2);
        dvorticity(2,1) = -dvorticity(1,2);

		// add total spin tensor in the particle
		particles->at(i)->setVorticityIncrement(dvorticity);
	}
}

void Interpolation::particleDeformationGradient(Mesh* mesh, vector<Particle*>* particles, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

#ifdef _OPENMP
    #pragma omp parallel for
#endif
    for (int i = 0; i < static_cast<int>(particles->size()); ++i)  {

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

// function to interpolate vector values in time
Eigen::Vector3d Interpolation::interpolateVector(const std::vector<double>& times, const std::vector<Eigen::Vector3d>& values, double itime) {
	
	// check if the time is out of bounds
	if (itime <= times.front()) {
		return Eigen::Vector3d::Zero();
	}
    
	if(itime >= times.back()) {
		// if the time is greater than the last time disable seismic analysis
		Seismic::disableSeismicAnalysis();
		return Eigen::Vector3d::Zero();
	}

	// find the upper limit where the time is located
	auto upper = std::upper_bound(times.begin(), times.end(), itime);
	size_t idx = std::distance(times.begin(), upper) - 1;

	// get the limit values in the interval
	double t0 = times[idx];
	double t1 = times[idx + 1];
	
	const Eigen::Vector3d v0 = values[idx];
	const Eigen::Vector3d v1 = values[idx + 1];

	// linear interpolation using limit values in the interval
	double factor = (itime - t0) / (t1 - t0);
	Eigen::Vector3d interpolatedValue = v0 + factor * (v1 - v0);

	return interpolatedValue;
}