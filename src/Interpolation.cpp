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
#include "Loads.h"
#include "TerrainContact.h"

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

	// The nodal external force is calculated from two sources,
	// one from the stored force in the particles (1), like gravity,
	// and two, from the external force imposed directly at the grid nodes (2)
	
	// (1) - External force from particles: f_ext_I = sum_p f_ext_p N_Ip

	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get nodes
		vector<Node*>* nodes = mesh->getNodes();

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

				// add weighted force in node
				nodeI->addExternalForce(pExtForce*contribution->at(j).getWeight());
			}
		}
	}

	// (2) - External force from nodes _ f_ext_I <- f_ext_I + f_ext_BC
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

void Interpolation::nodalDistanceLevelSet(Mesh* mesh, vector<Particle*>* particles) 
{
	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each particle
	for (size_t i = 0; i < particles->size(); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }

		// get nodes and weights that the particle contributes
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// get the particle mass
		const double pVolume = particles->at(i)->getInitialVolume();

		// for each node in the contribution list 
		for (size_t j = 0; j < contribution->size(); ++j) {

			// get the contributing node
			Node* nodeI = nodes->at(contribution->at(j).getNodeId());

			// compute and set the weighted density level set
			nodeI->addDensityLevelSet(pVolume*contribution->at(j).getWeight()/nodeI->getVolume());
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

// function to interpolate vector values in time
Eigen::Vector3d Interpolation::interpolateVector(const std::vector<double>& times, const std::vector<Eigen::Vector3d>& values, double itime) {
	
	// get the first and last values in the interval
	if (itime <= times.front()) return values.front();
	if (itime >= times.back()) return values.back();

	// find the upper limit where the time is located
	auto upper = std::upper_bound(times.begin(), times.end(), itime);
	size_t idx = std::distance(times.begin(), upper) - 1;

	// get the limit values in the interval
	double t0 = times[idx];
	double t1 = times[idx + 1];
	Eigen::Vector3d v0 = values[idx];
	Eigen::Vector3d v1 = values[idx + 1];

	// linear interpolation using limit values in the interval
	double factor = (itime - t0) / (t1 - t0);
	Eigen::Vector3d interpolatedValue = v0 + factor * (v1 - v0);

	return interpolatedValue;
}

void Interpolation::particleDistanceLevelSet(Mesh* mesh, vector<Particle*>* particles) {
    
	// get nodes
    vector<Node*>* nodes = mesh->getNodes();

    // iterate over all particles
    for (size_t i = 0; i < particles->size(); ++i) 
	{	
		// get the particle
        Particle* particle = particles->at(i);

        // only active particles are considered
        if (!particle->getActive()) { continue; }

        // get nodes and weights contributing to the particle
        const vector<Contribution>* contribution = particle->getContributionNodes();

        // initialize the interpolated distance level set value
        double interpolatedDistance = 0.0;

        // iterate over the nodal contribution list
        for (size_t j = 0; j < contribution->size(); ++j) {
            
			// get contributing node
            Node* nodeI = nodes->at(contribution->at(j).getNodeId());

            // get the nodal distance level set value
            double nodalDistance = nodeI->getDistanceLevelSet();

            // get the contribution weight
            double weight = contribution->at(j).getWeight();

            // accumulate the contribution
            interpolatedDistance += weight * nodalDistance;
        }

        // set interpolated value to particle
        particle->setDistanceLevelSet(interpolatedDistance);
    }
}

double interpolateDensityLevelSet(const Vector3d& position, const vector<int>& nodeIds, Mesh* mesh) {
    
    // get the nodes of the mesh
    vector<Node*>* gridNodes = mesh->getNodes();

	// get the nodes of the cell that contains the position
    Vector3d p0 = gridNodes->at(nodeIds[0])->getCoordinates();
    Vector3d p1 = gridNodes->at(nodeIds[1])->getCoordinates();
    Vector3d p2 = gridNodes->at(nodeIds[2])->getCoordinates();
    Vector3d p3 = gridNodes->at(nodeIds[3])->getCoordinates();
    Vector3d p4 = gridNodes->at(nodeIds[4])->getCoordinates();
    Vector3d p5 = gridNodes->at(nodeIds[5])->getCoordinates();
    Vector3d p6 = gridNodes->at(nodeIds[6])->getCoordinates();
    Vector3d p7 = gridNodes->at(nodeIds[7])->getCoordinates();

    // density level values of the nodes
    double d0 = gridNodes->at(nodeIds[0])->getDensityLevelSet();
    double d1 = gridNodes->at(nodeIds[1])->getDensityLevelSet();
    double d2 = gridNodes->at(nodeIds[2])->getDensityLevelSet();
    double d3 = gridNodes->at(nodeIds[3])->getDensityLevelSet();
    double d4 = gridNodes->at(nodeIds[4])->getDensityLevelSet();
    double d5 = gridNodes->at(nodeIds[5])->getDensityLevelSet();
    double d6 = gridNodes->at(nodeIds[6])->getDensityLevelSet();
    double d7 = gridNodes->at(nodeIds[7])->getDensityLevelSet();

/* 
       (6)------(7)  <- maxCorner = p7 (x_max, y_max, z_max)
       /|       /|
      / |      / |
    (4)------(5) |
     | (2)----|-(3)
     | /      | /
    (0)------(1)     <- minCorner = p0 (x_min, y_min, z_min)
*/

    // get the natural coordinates of the position
    
    // limit corners
    Vector3d minCorner = p0; // minimum corner (x_min,y_min,z_min)
    Vector3d maxCorner = p7; // maximum corner (x_max,y_max,z_max)

    // natural coordinates of the position
    double u = (2.0 * (position.x() - minCorner.x()) / (maxCorner.x() - minCorner.x())) - 1.0;
    double v = (2.0 * (position.y() - minCorner.y()) / (maxCorner.y() - minCorner.y())) - 1.0;
    double w = (2.0 * (position.z() - minCorner.z()) / (maxCorner.z() - minCorner.z())) - 1.0;

    // shape functions of the hexahedron
    double N0 = (1 - u) * (1 - v) * (1 - w) * 0.125;
    double N1 = (1 + u) * (1 - v) * (1 - w) * 0.125;
    double N2 = (1 - u) * (1 + v) * (1 - w) * 0.125;
    double N3 = (1 + u) * (1 + v) * (1 - w) * 0.125;
    double N4 = (1 - u) * (1 - v) * (1 + w) * 0.125;
    double N5 = (1 + u) * (1 - v) * (1 + w) * 0.125;
    double N6 = (1 - u) * (1 + v) * (1 + w) * 0.125;
    double N7 = (1 + u) * (1 + v) * (1 + w) * 0.125;

    // trilinear interpolation of the density level set
    double interpolatedDensity = N0 * d0 + N1 * d1 + N2 * d2 + N3 * d3 +
                                 N4 * d4 + N5 * d5 + N6 * d6 + N7 * d7;

    return interpolatedDensity;
}

void Interpolation::trianglesDensityLevelSet(Mesh* mesh, TerrainContact* terrainContact) {
    
    // get the triangles of the STL mesh
    const vector<Triangle>& triangles = terrainContact->getSTLMesh()->getTriangles();

    // create a vector to store the interpolated values
    vector<double> interpolatedValues(triangles.size(), 0.0);

    // iterate over all triangles
    #pragma omp parallel for shared(triangles, mesh, interpolatedValues)
    for (int i = 0; i < triangles.size(); ++i) {
        
        // get the current triangle
        const Triangle& triangle = triangles[i];

        // get the centroid of the triangle
        Vector3d centroid = triangle.getCentroid();

        // get the nodes of the cell that contains the centroid
        vector<int> nodeIds = mesh->getNodesInCell(centroid);

        // check if the cell is valid
        if (nodeIds.size() == 8) {
            // interpolate the density level set value to the centroid of the triangle
            interpolatedValues[i] = interpolateDensityLevelSet(centroid, nodeIds, mesh);
        } 
    }

    // store the interpolated values in the TerrainContact structure
    terrainContact->setDensityLevelSet(interpolatedValues);
}