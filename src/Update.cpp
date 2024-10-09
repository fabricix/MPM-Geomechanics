/*
 * Update.cpp
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Update.h"
#include "Loads.h"
#include <Interpolation.h>

void Update::nodalVelocity(Mesh* mesh) {

	// get nodes
	vector<Node*>* gNodes = mesh->getNodes();

	// for each node
	#pragma omp parallel for shared(gNodes) private(mesh)
	for (int i = 0; i < gNodes->size(); ++i)
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
	#pragma omp parallel for shared (gNodes) private(mesh)
	for (int i = 0; i < gNodes->size(); ++i) {

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
	#pragma omp parallel for shared (gNodes) private(mesh)
	for (int i = 0; i < gNodes->size(); ++i) {

		if(!gNodes->at(i)->getActive()){ continue; }

		// reset values
		gNodes->at(i)->resetValues();
	}
}

void Update::particleDensity(vector<Body*>* bodies) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		#pragma omp parallel for shared (particles) private(bodies, ibody)
		for (int i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }
			
			// update density
			particles->at(i)->updateDensity();
		}
	}
}

void Update::particlePorosity(vector<Body*>* bodies) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		#pragma omp parallel for shared (particles) private(bodies, ibody)
		for (int i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }
			
			// update density
			particles->at(i)->updatePorosity();
		}
	}
}

void Update::particleStress(vector<Body*>* bodies) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		#pragma omp parallel for shared (particles) private(bodies, ibody)
		for (int i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// update particle stress
			particles->at(i)->updateStress();
		}
	}
}

void Update::particlePressure(vector<Body*>* bodies, double dt) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		#pragma omp parallel for shared (particles) private(bodies, ibody)
		for (int i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }

			// update particle stress
			particles->at(i)->updatePressure(dt);
		}
	}
	// set up prescribed pore pressure
	Loads::updatePrescribedPorePressure(bodies);
}

void Update::particleVelocity(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle 
		#pragma omp parallel for shared (particles, nodes, dt) private(mesh, bodies, ibody)
		for (int i = 0; i < particles->size(); ++i) {

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
}

void Update::particleVelocityFluid(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle 
		#pragma omp parallel for shared (particles, nodes, dt) private(mesh, bodies, ibody)
		for (int i = 0; i < particles->size(); ++i) {

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
}

void Update::particlePosition(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		#pragma omp parallel for shared(particles, nodes, dt) private(ibody, bodies, mesh)
		for (int i = 0; i < particles->size(); ++i) {

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
}

void Update::setPlaneMomentum(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

	double dt = ModelSetup::getTimeStep();
	Eigen::Vector3d interpolatedAcceleration = ModelSetup::getSeismicAnalysis() ? Interpolation::interpolateVector(Loads::getSeismicData().time, Loads::getSeismicData().acceleration, ModelSetup::getCurrentTime()) : Vector3d{0,0,0} ;
	Eigen::Vector3d interpolatedVelocity = ModelSetup::getSeismicAnalysis() ? Interpolation::interpolateVector(Loads::getSeismicData().time, Loads::getSeismicData().velocity, ModelSetup::getCurrentTime()) : Vector3d{0,0,0} ;

	// for each boundary node
	#pragma omp parallel for shared(plane, nodes, dir)
	for (int i = 0; i < plane->nodes.size(); ++i){

		// get node handle
		Node* nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI->getActive()) {

			// witch type of restriction
			switch(plane->restriction) 
			{
				// free condition
				case Boundary::BoundaryType::FREE:
				{
					break;
				}
				// fixed condition
				case Boundary::BoundaryType::FIXED:
				{
					// set all momentum components as zero
					nodeI->setMomentum(Vector3d::Zero());
					break;
				}
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

static void setPlaneEarthquakeMomentum(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

	if(plane->restriction != Boundary::BoundaryType::EARTHQUAKE) return;

	Eigen::Vector3d interpolatedVelocity = ModelSetup::getSeismicAnalysis() ? Interpolation::interpolateVector(Loads::getSeismicData().time, Loads::getSeismicData().velocity, ModelSetup::getCurrentTime()) : Vector3d{0,0,0} ;

	#pragma omp parallel for shared(plane, nodes)
	for (int i = 0; i < plane->nodes.size(); ++i){

		Node* nodeI = nodes->at(plane->nodes.at(i));
	
		nodeI->setMomentum(nodeI->getMass() * interpolatedVelocity);
	}
}

void Update::setPlaneMomentumFluid(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

	// for each boundary node
	#pragma omp parallel for shared(plane, nodes, dir)
	for (int i = 0; i < plane->nodes.size(); ++i){

		// get node handle
		Node* nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI->getActive()) {

			// witch type of restriction of fluid
			switch(plane->restrictionFluid) {

				// free condition
				case Boundary::BoundaryType::FREE:
					break;

				// fixed condition
				case Boundary::BoundaryType::FIXED:

					// set all momentum components as zero
					nodeI->setMomentumFluid(Vector3d::Zero());
					break;
				
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

	// plane X0 is the plane passing for the origin and points to -x axes
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);

	// plane Y0 is the plane passing for the origin and points to -y axes
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);

	// plane Z0 is the plane passing for the origin and points to -z axes
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);
	
	// plane Xn is the plane passing for the maximum x coordinate and points to x axes
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);

	// plane Yn is the plane passing for the maximum y coordinate and points to y axes
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);

	// plane Zn is the plane passing for the maximum z coordinate and points to z axes
	setPlaneMomentumFluid(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);
}

void Update::boundaryConditionsMomentum(Mesh* mesh) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// plane X0 is the plane passing for the origin and points to -x axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);

	// plane Y0 is the plane passing for the origin and points to -y axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);

	// plane Z0 is the plane passing for the origin and points to -z axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);
	
	// plane Xn is the plane passing for the maximum x coordinate and points to x axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);

	// plane Yn is the plane passing for the maximum y coordinate and points to y axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);

	// plane Zn is the plane passing for the maximum z coordinate and points to z axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);

	// set earthquake boundary condition
	setPlaneEarthquakeMomentum(mesh->getBoundary()->getPlaneX0(), nodes);
	setPlaneEarthquakeMomentum(mesh->getBoundary()->getPlaneXn(), nodes);
	setPlaneEarthquakeMomentum(mesh->getBoundary()->getPlaneY0(), nodes);
	setPlaneEarthquakeMomentum(mesh->getBoundary()->getPlaneYn(), nodes);
	setPlaneEarthquakeMomentum(mesh->getBoundary()->getPlaneZ0(), nodes);
	setPlaneEarthquakeMomentum(mesh->getBoundary()->getPlaneZn(), nodes);
} 

void Update::setPlaneForce(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

	// get boundary nodes
	#pragma omp parallel for shared(plane, nodes, dir)
	for (int i = 0; i < plane->nodes.size(); ++i) {

		// get node handle 
		Node* nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI->getActive()) {
			
			// witch type of restriction
			switch(plane->restriction)
			{
				// free condition
				case Boundary::BoundaryType::FREE:
				{
					break;
				}
				// fixed condition
				case Boundary::BoundaryType::FIXED:
				{
					// set all force component as zero 
					nodeI->setTotalForce(Vector3d::Zero());
					break;
				}
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
			}
		}
	}
}

static void setPlaneEarthquakeForce(const Boundary::planeBoundary* plane, vector<Node*>* nodes) {

	if(plane->restriction != Boundary::BoundaryType::EARTHQUAKE) return;

	Eigen::Vector3d interpolatedAcceleration = ModelSetup::getSeismicAnalysis() ? Interpolation::interpolateVector(Loads::getSeismicData().time, Loads::getSeismicData().acceleration, ModelSetup::getCurrentTime()) : Vector3d{0,0,0} ;

	// get boundary nodes
	#pragma omp parallel for shared(plane, nodes)
	for (int i = 0; i < plane->nodes.size(); ++i) {

		Node* nodeI = nodes->at(plane->nodes.at(i));

		nodeI->setTotalForce(nodeI->getMass() * interpolatedAcceleration);
	}
}

void Update::boundaryConditionsForce(Mesh* mesh) {

	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// plane X0 is the plane passing for the origin and points to -x axes
	setPlaneForce(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);

	// plane Y0 is the plane passing for the origin and points to -y axes
	setPlaneForce(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);

	// plane Z0 is the plane passing for the origin and points to -z axes
	setPlaneForce(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);
	
	// plane Xn is the plane passing for the maximum x coordinate and points to x axes
	setPlaneForce(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);

	// plane Yn is the plane passing for the maximum x coordinate and points to y axes
	setPlaneForce(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);

	// plane Zn is the plane passing for the maximum x coordinate and points to z axes
	setPlaneForce(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);

	// earthquake boudaty condition
	setPlaneEarthquakeForce(mesh->getBoundary()->getPlaneX0());
	setPlaneEarthquakeForce(mesh->getBoundary()->getPlaneXn());
	setPlaneEarthquakeForce(mesh->getBoundary()->getPlaneY0());
	setPlaneEarthquakeForce(mesh->getBoundary()->getPlaneYn());
	setPlaneEarthquakeForce(mesh->getBoundary()->getPlaneZ0());
	setPlaneEarthquakeForce(mesh->getBoundary()->getPlaneZn());
}

void Update::setPlaneForceFluid(const Boundary::planeBoundary* plane, vector<Node*>* nodes, unsigned dir) {

	// get boundary nodes
	#pragma omp parallel for shared(plane, nodes, dir)
	for (int i = 0; i < plane->nodes.size(); ++i) {

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

	// plane X0 is the plane passing for the origin and points to -x axes
	setPlaneForceFluid(mesh->getBoundary()->getPlaneX0(), nodes, Update::Direction::X);

	// plane Y0 is the plane passing for the origin and points to -y axes
	setPlaneForceFluid(mesh->getBoundary()->getPlaneY0(), nodes, Update::Direction::Y);

	// plane Z0 is the plane passing for the origin and points to -z axes
	setPlaneForceFluid(mesh->getBoundary()->getPlaneZ0(), nodes, Update::Direction::Z);
	
	// plane Xn is the plane passing for the maximum x coordinate and points to x axes
	setPlaneForceFluid(mesh->getBoundary()->getPlaneXn(), nodes, Update::Direction::X);

	// plane Yn is the plane passing for the maximum x coordinate and points to y axes
	setPlaneForceFluid(mesh->getBoundary()->getPlaneYn(), nodes, Update::Direction::Y);

	// plane Zn is the plane passing for the maximum x coordinate and points to z axes
	setPlaneForceFluid(mesh->getBoundary()->getPlaneZn(), nodes, Update::Direction::Z);
}

void Update::contributionNodes(Mesh* mesh, vector<Body*>* bodies) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();
		
		// for each particle
		#pragma omp parallel for shared(particles, mesh) private(bodies, ibody)
		for (int i = 0; i < particles->size(); ++i) {

			// only active particle can contribute
			if (!particles->at(i)->getActive()) { continue; }
			
			// update the contribution nodes
			particles->at(i)->updateContributionNodes(mesh);
		}
	}
}

static Eigen::Vector3d getNormal(Update::Direction dir) {
	Eigen::Vector3d normal = Eigen::Vector3d::Zero();

	switch (dir) {
	case Update::Direction::X:
		normal.x() = 1.0;
		break;
	case Update::Direction::Y:
		normal.y() = 1.0;
		break;
	case Update::Direction::Z:
		normal.z() = 1.0;
		break;
	default:
		// additional cases
		break;
	}
	return normal;
}