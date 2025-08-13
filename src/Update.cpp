// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Update.h"
#include "Loads.h"
#include "Interpolation.h"
#include "Seismic.h"

void Update::nodalVelocity(Mesh* mesh) {

	// get nodes
	vector<Node*>* gNodes = mesh->getNodes();

	// for each node
#ifdef _OPENMP
	#pragma omp parallel for shared(gNodes)
#endif
	for (int i = 0; i < static_cast<int>(gNodes->size()); ++i)
	{	
		if(!gNodes->at(i)->getActive()){ continue; }
		
		// update the velocity
		gNodes->at(i)->updateVelocity();
	}
}

void Update::nodalTotalForce(Mesh* mesh) {

	// get nodes
	vector<Node*>* gNodes = mesh->getNodes();

	// for each node
#ifdef _OPENMP
	#pragma omp parallel for shared (gNodes)
#endif
	for (int i = 0; i < static_cast<int>(gNodes->size()); ++i) {

		if(!gNodes->at(i)->getActive()){ continue; }

		// update damping forces
		gNodes->at(i)->updateDampingForce();
			
		// update total forces
		gNodes->at(i)->updateTotalForce();
	}
}

void Update::resetNodalValues(Mesh* mesh) {

	// get nodes
	vector<Node*>* gNodes = mesh->getNodes();

	// for each node
#ifdef _OPENMP
	#pragma omp parallel for shared (gNodes)
#endif
	for (int i = 0; i < static_cast<int>(gNodes->size()); ++i) {

		if(!gNodes->at(i)->getActive()){ continue; }

		// reset values
		gNodes->at(i)->resetValues();
	}
}

void Update::resetNodalMomentum(Mesh* mesh) {

	// get nodes
	vector<Node*>* gNodes = mesh->getNodes();

	// for each node
#ifdef _OPENMP
	#pragma omp parallel for shared (gNodes)
#endif
	for (int i = 0; i < static_cast<int>(gNodes->size()); ++i) {

		if(!gNodes->at(i)->getActive()){ continue; }

		// explicitly nullify momentum
		gNodes->at(i)->setMomentum(Vector3d::Zero());
	}
}

void Update::particleDensity(vector<Particle*>* particles) {
	
#ifdef _OPENMP
	#pragma omp parallel for shared(particles)
#endif
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }
		
		// update density
		particles->at(i)->updateDensity();
	}
}

void Update::particlePorosity(vector<Particle*>* particles) {

#ifdef _OPENMP
		#pragma omp parallel for shared (particles)
#endif
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }
		
		// update density
		particles->at(i)->updatePorosity();
	}
}

void Update::particleStress(vector<Particle*>* particles) {

#ifdef _OPENMP
	#pragma omp parallel for shared (particles)
#endif
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }

		// update particle stress
		particles->at(i)->updateStress();
	}
}

void Update::particlePressure(vector<Body*>* bodies, double dt) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
#ifdef _OPENMP
		#pragma omp parallel for shared (particles)
#endif
		for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// update particle stress
			particles->at(i)->updatePressure(dt);
		}
	}
	// set up prescribed pore pressure
	Loads::updatePrescribedPorePressure(bodies);
}

void Update::particleVelocity(Mesh* mesh, vector<Particle*>* particles, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each particle
#ifdef _OPENMP
	#pragma omp parallel for shared (particles, nodes, dt)
#endif
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }

		// get nodes and weights that the particle contributes
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// initialize the velocity rate vector
		Vector3d velocityRate = Vector3d::Zero();
		
		// for each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get the contributing node structure
			const Contribution contribI = contribution->at(j);

			// get the contributing node handle
			Node* nodeI = nodes->at(contribI.getNodeId());

			if (nodeI->getMass()!=0.0) {

				// compute the velocity rate contribution
				velocityRate+=nodeI->getTotalForce()*contribI.getWeight()/nodeI->getMass();
			}
		}

		// get particle handle
		Particle* particleP = particles->at(i);

		// update particle velocity
		particleP->setVelocity(particleP->getVelocity()+velocityRate*dt);
	}
}

void Update::particleVelocityFluid(Mesh* mesh, vector<Particle*>* particles, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

		// for each particle
#ifdef _OPENMP 
		#pragma omp parallel for shared (particles, nodes, dt)
#endif
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }

		// get nodes and weights that the particle contributes
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// initialize the velocity rate vector
		Vector3d velocityRate = Vector3d::Zero();
		
		// for each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get the contributing node structure
			const Contribution contribI = contribution->at(j);

			// get the contributing node handle
			Node* nodeI = nodes->at(contribI.getNodeId());

			if (nodeI->getMassFluid()!=0.0) {

				// compute the velocity rate contribution
				velocityRate+= (*(nodeI->getTotalForceFluid()))*contribI.getWeight()/nodeI->getMassFluid();
			}
		}

		// get particle handle
		Particle* particleP = particles->at(i);

		// update particle velocity
		particleP->setVelocityFluid((*particleP->getVelocityFluid())+velocityRate*dt);
	}
}

void Update::particlePosition(Mesh* mesh, vector<Particle*>* particles, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each particle
#ifdef _OPENMP
	#pragma omp parallel for shared(particles, nodes, dt)
#endif
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }

		// get nodes and weights that the particle contributes
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// initialize the position rate vector
		Vector3d positionRate = Vector3d::Zero();
		
		// for each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get the contributing structure
			const Contribution contribI = contribution->at(j);

			// get the contributing node
			Node* nodeI = nodes->at(contribI.getNodeId());

			if (nodeI->getMass()!=0.0){

				// compute the position rate contribution
				positionRate+=nodeI->getMomentum()*contribI.getWeight()/nodeI->getMass();
			}
		}

		// get particle handle
		Particle* particleP = particles->at(i);

		// update particle position
		particleP->setPosition(particleP->getPosition()+positionRate*dt);
	}
}

void Update::setPlaneMomentum(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

#ifdef _OPENMP
	#pragma omp parallel for shared(plane, nodes, dir)
#endif
	for (int i = 0; i < static_cast<int>(plane->nodes.size()); ++i){

		// get node handle
		Node* nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI->getActive()) {

			// witch type of restriction
			switch(plane->restriction) 
			{
				// free condition
				case Boundary::BoundaryType::FREE: { break; }
				
				// earthquake condition
				case Boundary::BoundaryType::EARTHQUAKE:
				{ 	
					// set the seismic momentum
					if(ModelSetup::getUpdateStressScheme() == ModelSetup::StressUpdateScheme::MUSL) {
                		nodeI->setMomentum(nodeI->getMass()*Seismic::getAccumulatedVelocity());
					}
					break;
				}

				// fixed condition
				case Boundary::BoundaryType::FIXED:
				{
					// set all momentum components as zero
					nodeI->setMomentum(Vector3d::Zero());
					break;
				}

				// absorbing condition
				case Boundary::BoundaryType::ABSORBING:
				// fall through: treat as SLIDING for now

				// sliding restriction
				case Boundary::BoundaryType::SLIDING:
				{
					// get current boundary nodal momentum
					Vector3d momentum = nodeI->getMomentum();

					// witch direction of the normal vector
					switch (dir) 
					{
						// normal pointed to x
						case Update::Direction::X:
						{
							momentum.x() = 0.0;
							break;
						}
						// normal pointed to y
						case Update::Direction::Y:
						{
							momentum.y() = 0.0;
							break;
						}
						// normal pointed to z
						case Update::Direction::Z:
						{
							momentum.z() = 0.0;
							break;
						}
					}

					// set the boundary nodal momentum
					nodeI->setMomentum(momentum);
					break;
				}
			}
		}
	}
}

void Update::setPlaneMomentumFluid(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

#ifdef _OPENMP
	#pragma omp parallel for shared(plane, nodes, dir)
#endif
	for (int i = 0; i < static_cast<int>(plane->nodes.size()); ++i){

		// get node handle
		Node* nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI->getActive()) {

			// witch type of restriction of fluid
			switch(plane->restrictionFluid) {

				// free condition
				case Boundary::BoundaryType::FREE:
					break;

				// earthquake condition
				case Boundary::BoundaryType::EARTHQUAKE: 
					break;

				// fixed condition
				case Boundary::BoundaryType::FIXED:

					// set all momentum components as zero
					nodeI->setMomentumFluid(Vector3d::Zero());
					break;
				
				// Absorbing condition
				case Boundary::BoundaryType::ABSORBING:
				// fall through: treat as SLIDING for now

				// perpendicular restriction
				case Boundary::BoundaryType::SLIDING:
				{	
					// get current boundary nodal momentum of fluid
					Vector3d momentum = *(nodeI->getMomentumFluid());
					
					// witch direction of the normal vector
					switch(dir) {

						// normal pointed to x
						case Update::Direction::X :
							momentum.x()=0.0;
							break;

						// normal pointed to y
						case Update::Direction::Y :
							momentum.y()=0.0;
							break;

						// normal pointed to z
						case Update::Direction::Z :
							momentum.z()=0.0;
							break;
					}

					// set the boundary nodal momentum
					nodeI->setMomentumFluid(momentum);
					break;
				}
			}
		}
	}
}

void Update::boundaryConditionsMomentumFluid(Mesh* mesh) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// set p=0 in fixed nodes

	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);
	
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);
}

void Update::boundaryConditionsMomentum(Mesh* mesh) 
{
	// Verify if seismic was disabled during the simulation.
	// This can happen if the simulation time is greater than the seismic data time.
	if (!ModelSetup::getSeismicAnalysisActive() && mesh->getBoundary()->getPlaneZ0()->restriction == Boundary::BoundaryType::EARTHQUAKE) {
		// change EARTHQUAKE to FIXED for the mesh boundary
		mesh->getBoundary()->setRestrictions(Boundary::BoundaryType::FIXED);
	}

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// set p = 0 in fixed direction

	setPlaneMomentum(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);
	setPlaneMomentum(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);
	setPlaneMomentum(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);

	setPlaneMomentum(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);
	setPlaneMomentum(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);
	setPlaneMomentum(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);

} 

/// @brief Set force boundary conditions in the specified plane
/// @details This function applies boundary conditions to the nodal forces based on the specified plane
void Update::setPlaneForce( const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) 
{
	// get boundary nodes
#ifdef _OPENMP
	#pragma omp parallel for shared(plane, nodes, dir)
#endif
	for (int i = 0; i < static_cast<int>(plane->nodes.size()); ++i) {

		// get node handle 
		Node* nodeI = nodes->at(plane->nodes.at(i));

		// check if the node is active
		// and apply the boundary condition based on the restriction type
		if (nodeI->getActive()) {
			
			// witch type of restriction
			switch(plane->restriction)
			{
				// free condition
				case Boundary::BoundaryType::FREE: { break; }
				
				// fixed condition f_iI = 0
				case Boundary::BoundaryType::FIXED:
				{
					// set all force component as zero 
					nodeI->setTotalForce(Vector3d::Zero());
					break;
				}

				// absorbing condition
				case Boundary::BoundaryType::ABSORBING:
				// fall through: treat as SLIDING for now

				// perpendicular restriction
				case Boundary::BoundaryType::SLIDING:
				{
					// get current boundary nodal force
					Vector3d force = nodeI->getTotalForce();
					
					// witch direction of the normal vector
					switch(dir)
					{
						// normal pointed to x
						case Update::Direction::X :
						{
							force.x()=0.0;
							break;
						}
						// normal pointed to y
						case Update::Direction::Y :
						{
							force.y()=0.0;
							break;
						}
						// normal pointed to z
						case Update::Direction::Z :
						{
							force.z()=0.0;
							break;
						}
					}

					// set boundary nodal force
					nodeI->setTotalForce(force);
					break;
				}
				// Earthquake boundary condition in term of force
				case Boundary::BoundaryType::EARTHQUAKE:
				{ 
					nodeI->setTotalForce(Seismic::getAcceleration()*nodeI->getMass());
					break;
				}
			}
		}
	}
}

/// @brief Set force boundary conditions
/// @param mesh Pointer to the mesh object 
void Update::boundaryConditionsForce(Mesh* mesh)
{	
	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// set f = 0 in fixed direction
	setPlaneForce(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);
	setPlaneForce(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);
	setPlaneForce(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);

	setPlaneForce(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);
	setPlaneForce(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);
	setPlaneForce(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);

}

void Update::setPlaneForceFluid(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

	// get boundary nodes
#ifdef _OPENMP
	#pragma omp parallel for shared(plane, nodes, dir)
#endif
	for (int i = 0; i < static_cast<int>(plane->nodes.size()); ++i) {

		// get node handle 
		Node* nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI->getActive()) {
			
			// witch type of restriction
			switch(plane->restrictionFluid) {

				// free condition
				case Boundary::BoundaryType::FREE:
					break;

				// fixed condition
				case Boundary::BoundaryType::FIXED:

					// set all force component as zero 
					nodeI->setTotalForceFluid(Vector3d::Zero());
					break;
				
				// earthquake boundary condition
				case Boundary::BoundaryType::EARTHQUAKE:
				{ 
					nodeI->setTotalForceFluid(Eigen::Vector3d::Zero());
					break;
				}

				// absorbing condition
				case Boundary::BoundaryType::ABSORBING:
				// fall through: treat as SLIDING for now

				// perpendicular restriction
				case Boundary::BoundaryType::SLIDING:
					
					// get current boundary nodal force of fluid phase
					Vector3d force = *(nodeI->getTotalForceFluid());
					
					// witch direction of the normal vector
					switch(dir) {

						// normal pointed to x
						case Update::Direction::X :
							force.x()=0.0;
							break;

						// normal pointed to y
						case Update::Direction::Y :
							force.y()=0.0;
							break;

						// normal pointed to z
						case Update::Direction::Z :
							force.z()=0.0;
							break;
					}

					// set boundary nodal force in fluid phase
					nodeI->setTotalForceFluid(force);
					break;
			}
		}
	}
}

void Update::boundaryConditionsForceFluid(Mesh* mesh) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();
		
	// set f=0 in fixed nodes
	setPlaneForceFluid(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);
	setPlaneForceFluid(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);
	setPlaneForceFluid(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);
	
	setPlaneForceFluid(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);
	setPlaneForceFluid(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);
	setPlaneForceFluid(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);
}

void Update::contributionNodes(Mesh* mesh, vector<Particle*>* particles) 
{
	// for each particle
#ifdef _OPENMP
	#pragma omp parallel for shared(particles, mesh)
#endif
	for (int i = 0; i < static_cast<int>(particles->size()); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }
		
		// update the contribution nodes
		particles->at(i)->updateContributionNodes(mesh);
	}
}