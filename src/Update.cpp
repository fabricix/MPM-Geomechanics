/*
 * Update.cpp
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Update.h"

void Update::nodalVelocity(Mesh* mesh) {

	// get nodes
	vector<Node>* gNodes = mesh->getNodes();

	// for each node
	for (size_t i = 0; i < gNodes->size(); ++i)
	{	
		// get node handle
		Node& nodeI = gNodes->at(i);

		if(nodeI.getActive()){
		
			// update the velocity
			nodeI.updateVelocity();
		}
	}
}

void Update::nodalTotalForce(Mesh* mesh) {

	// get nodes
	vector<Node>* gNodes = mesh->getNodes();

	// for each node
	for (size_t i = 0; i < gNodes->size(); ++i) {

		// get node handle
		Node& nodeI = gNodes->at(i);

		if(nodeI.getActive()) {

			// update damping forces
			nodeI.updateDampingForce();
			
			// update total forces
			nodeI.updateTotalForce();
		}
	}
}

void Update::resetNodalValues(Mesh* mesh) {

	// get nodes
	vector<Node>* gNodes = mesh->getNodes();

	// for each node
	for (size_t i = 0; i < gNodes->size(); ++i) {

		// get node handle
		Node& nodeI = gNodes->at(i);

		if(nodeI.getActive()) {
			
			// reset values
			nodeI.resetValues();
		}
	}
}

void Update::particleDensity(vector<Body*>* bodies) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle 
		for (size_t i = 0; i < particles->size(); ++i) {

			// update density
			particles->at(i)->updateDensity();
		}
	}
}

void Update::particleStress(vector<Body*>* bodies) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle 
		for (size_t i = 0; i < particles->size(); ++i) {

			// update particle stress
			particles->at(i)->updateStress();
		}
	}
}

void Update::particleVelocity(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle 
		for (size_t i = 0; i < particles->size(); ++i) {

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
				Node& nodeI = nodes->at(contribI.getNodeId());

				if (nodeI.getMass()!=0.0) {

					// compute the velocity rate contribution
					velocityRate+=nodeI.getTotalForce()*contribI.getWeight()/nodeI.getMass();
				}
			}

			// get particle handle
			Particle* particleP = particles->at(i);

			// update particle velocity
			particleP->setVelocity(particleP->getVelocity()+velocityRate*dt);
		}
	}
}

void Update::particlePosition(Mesh* mesh, vector<Body*>* bodies, double dt) {

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();

		// for each particle
		for (size_t i = 0; i < particles->size(); ++i) {

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
				Node& nodeI = nodes->at(contribI.getNodeId());

				if (nodeI.getMass()!=0.0){

					// compute the position rate contribution
					positionRate+=nodeI.getMomentum()*contribI.getWeight()/nodeI.getMass();
				}
			}

			// get particle handle
			Particle* particleP = particles->at(i);

			// update particle position
			particleP->setPosition(particleP->getPosition()+positionRate*dt);
		}
	}
}

void Update::setPlaneMomentum(const Boundary::planeBoundary* plane, vector<Node>* nodes, unsigned dir) {

	// for each boundary node
	for (size_t i = 0; i < plane->nodes.size(); ++i){

		// get node handle
		Node& nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI.getActive()) {

			// witch type of restriction
			switch(plane->type) {

				// fixed condition
				case Boundary::BoundaryType::FIXED:

					// set all momentum components as zero
					nodeI.setMomentum(Vector3d::Zero());
				
				break;
				
				// perpendicular restriction
				case Boundary::BoundaryType::SLIDING:
					
					// get current boundary nodal momentum
					Vector3d momentum = nodeI.getMomentum();
					
					// witch direction of the normal vector
					switch(dir) {

						// normal pointed to x
						case 0:
							momentum.x()=0.0;
							break;

						// normal pointed to y
						case 1:
							momentum.y()=0.0;
							break;

						// normal pointed to z
						case 2:
							momentum.z()=0.0;
							break;
					}

					// set the boundary nodal momentum
					nodeI.setMomentum(momentum);

				break;
			}
		}
	}
}

void Update::boundaryConditionsMomentum(Mesh* mesh) {

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

	// use 0 for impose restrictions in x direction
	// use 1 for impose restrictions in y direction
	// use 2 for impose restrictions in z direction

	// plane X0 is the plane passing for the origin and points to -x axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneX0(), nodes, 0);

	// plane Y0 is the plane passing for the origin and points to -y axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneY0(), nodes, 1);

	// plane Z0 is the plane passing for the origin and points to -z axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneZ0(), nodes, 2);
	
	// plane Xn is the plane passing for the maximum x coordinate and points to x axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneXn(), nodes, 0);

	// plane Yn is the plane passing for the maximum y coordinate and points to y axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneYn(), nodes, 1);

	// plane Zn is the plane passing for the maximum z coordinate and points to z axes
	setPlaneMomentum(mesh->getBoundary()->getPlaneZn(), nodes, 2);
}

void Update::setPlaneForce(const Boundary::planeBoundary* plane, vector<Node>* nodes, unsigned dir) {

	// get boundary nodes
	for (size_t i = 0; i < plane->nodes.size(); ++i) {

		// get node handle 
		Node& nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI.getActive()) {
			
			// witch type of restriction
			switch(plane->type) {

				// fixed condition
				case Boundary::BoundaryType::FIXED:

					// set all force component as zero 
					nodeI.setTotalForce(Vector3d::Zero());
					break;
				
				// perpendicular restriction
				case Boundary::BoundaryType::SLIDING:
					
					// get current boundary nodal force
					Vector3d force = nodeI.getTotalForce();
					
					// witch direction of the normal vector
					switch(dir) {

						// normal pointed to x
						case 0:
							force.x()=0.0;
							break;

						// normal pointed to y
						case 1:
							force.y()=0.0;
							break;

						// normal pointed to z
						case 2:
							force.z()=0.0;
							break;
					}

					// set boundary nodal force
					nodeI.setTotalForce(force);
					break;
			}
		}
	}
}

void Update::boundaryConditionsForce(Mesh* mesh) {

	// get nodes
	vector<Node>* nodes = mesh->getNodes();

	// use 0 for impose restrictions in x direction
	// use 1 for impose restrictions in y direction
	// use 2 for impose restrictions in z direction

	// plane X0 is the plane passing for the origin and points to -x axes
	setPlaneForce(mesh->getBoundary()->getPlaneX0(), nodes, 0);

	// plane Y0 is the plane passing for the origin and points to -y axes
	setPlaneForce(mesh->getBoundary()->getPlaneY0(), nodes, 1);

	// plane Z0 is the plane passing for the origin and points to -z axes
	setPlaneForce(mesh->getBoundary()->getPlaneZ0(), nodes, 2);
	
	// plane Xn is the plane passing for the maximum x coordinate and points to x axes
	setPlaneForce(mesh->getBoundary()->getPlaneXn(), nodes, 0);

	// plane Yn is the plane passing for the maximum x coordinate and points to y axes
	setPlaneForce(mesh->getBoundary()->getPlaneYn(), nodes, 1);

	// plane Zn is the plane passing for the maximum x coordinate and points to z axes
	setPlaneForce(mesh->getBoundary()->getPlaneZn(), nodes, 2);
}

void Update::contributionNodes(Mesh* mesh, vector<Body*>* bodies) {

	// for each body
	for (size_t ibody = 0; ibody < bodies->size(); ++ibody) {

		// get particles
		vector<Particle*>* particles = bodies->at(ibody)->getParticles();
		
		// for each particle 
		for (size_t i = 0; i < particles->size(); ++i) {

			// update the contribution nodes
			particles->at(i)->updateContributionNodes(mesh);
		}
	}
}
