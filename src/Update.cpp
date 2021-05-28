/*
 * Update.cpp
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Update.h"

void Update::nodalVelocity(Mesh& mesh){

	// reference to grid nodes
	vector<Node>* gNodes = mesh.getNodes();

	// update the nodal velocity
	for (size_t i = 0; i < gNodes->size(); ++i)
	{
		Node& nodeI = gNodes->at(i);

		if(nodeI.getActive() && nodeI.getMass()!=0.0){
		
			nodeI.setVelocity(nodeI.getMomentum()/nodeI.getMass());
		}
	}
}

void Update::nodalTotalForce(Mesh& mesh){

	// reference to grid nodes
	vector<Node>* gNodes = mesh.getNodes();

	// over all nodes
	for (size_t i = 0; i < gNodes->size(); ++i)
	{
		Node& nodeI = gNodes->at(i);

		if(nodeI.getActive()){
		
			nodeI.setTotalForce(nodeI.getInternalForce()+nodeI.getExternalForce());
		}
	}
}

void Update::resetNodalValues(Mesh& mesh){

	// reference to grid nodes
	vector<Node>* gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < gNodes->size(); ++i)
	{
		Node& nodeI = gNodes->at(i);

		if(nodeI.getActive()){
			
			nodeI.resetValues();
		}
	}
}

void Update::particleDensity(vector<Particle*>* particles){

	// For each particle 
	for (size_t i = 0; i < particles->size(); ++i)
	{
		Particle* particleP = particles->at(i);

		// volumetric strain increment
		double volStrainInc = particleP->getStrainIncrement().trace();
		
		// update particle density
		if ((1+volStrainInc)!=0){

			particleP->setDensity(particleP->getDensity()/(1+volStrainInc));
		}
	}
}

void Update::particleStress(vector<Particle*>* particles){

	// For each particle 
	for (size_t i = 0; i < particles->size(); ++i)
	{
		// update particle stress
		particles->at(i)->updateStress();
	}
}

void Update::particleVelocity(Mesh& mesh, vector<Particle*>* particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// initialize the result vector
		Vector3d velocityRate = Vector3d::Zero();
		
		// For each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// contributing node structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// compute the velocity rate contributuion
			if (nodeI.getMass()!=0.0)
			{	
				velocityRate+=nodeI.getTotalForce()*contribI.getWeight()/nodeI.getMass();
			}
		}

		// particle pointer
		Particle* particleP = particles->at(i);

		// update particle velocity
		particleP->setVelocity(particleP->getVelocity()+velocityRate*dt);
	}
}

void Update::particlePosition(Mesh& mesh, vector<Particle*>* particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// initialize the result vector
		Vector3d positionRate = Vector3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// contributing node structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// compute the position rate contribution
			if (nodeI.getMass()!=0.0)
			{
				positionRate+=nodeI.getMomentum()*contribI.getWeight()/nodeI.getMass();
			}
		}

		// particle pointer
		Particle* particleP = particles->at(i);

		// update particle position
		particleP->setPosition(particleP->getPosition()+positionRate*dt);
	}
}

void Update::setPlaneMomentum(const Boundary::planeBoundary* plane, vector<Node>* nodes, int dir){

	for (size_t i = 0; i < plane->nodes.size(); ++i)
	{
		Node& nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI.getActive()){

			switch(plane->type) {

				case Boundary::BoundaryType::FIXED:
					nodeI.setMomentum(Vector3d::Zero());
				break;
			
				case Boundary::BoundaryType::SLIDING:
					
					Vector3d momentum = nodeI.getMomentum();
					
					switch(dir) {

						case 0:
						momentum.x()=0.0;
						break;

						case 1:
						momentum.y()=0.0;
						break;

						case 2:
						momentum.z()=0.0;
						break;
					}

					nodeI.setMomentum(momentum);

				break;
			}
		}
	}
}

void Update::boundaryConditionsMomentum(Mesh& mesh){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// x=0, y=1, z=2

	// Plane X0
	setPlaneMomentum(mesh.getBoundary()->getPlaneX0(), nodes, 0);

	// Plane Y0
	setPlaneMomentum(mesh.getBoundary()->getPlaneY0(), nodes, 1);

	// Plane Z0
	setPlaneMomentum(mesh.getBoundary()->getPlaneZ0(), nodes, 2);
	
	// Plane Xn
	setPlaneMomentum(mesh.getBoundary()->getPlaneXn(), nodes, 0);

	// Plane Yn
	setPlaneMomentum(mesh.getBoundary()->getPlaneYn(), nodes, 1);

	// Plane Zn
	setPlaneMomentum(mesh.getBoundary()->getPlaneZn(), nodes, 2);
}

void Update::setPlaneForce(const Boundary::planeBoundary* plane, vector<Node>* nodes, int dir){

	for (size_t i = 0; i < plane->nodes.size(); ++i)
	{
		Node& nodeI = nodes->at(plane->nodes.at(i));

		if (nodeI.getActive()){
			
			switch(plane->type) {

				case Boundary::BoundaryType::FIXED:
					nodeI.setTotalForce(Vector3d::Zero());
				break;
			
				case Boundary::BoundaryType::SLIDING:
					
					Vector3d force = nodeI.getTotalForce();
					
					switch(dir) {

						case 0:
						force.x()=0.0;
						break;

						case 1:
						force.y()=0.0;
						break;

						case 2:
						force.z()=0.0;
						break;
					}
					nodeI.setTotalForce(force);
				break;
			}
		}
	}
}

void Update::boundaryConditionsForce(Mesh& mesh){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// x=0, y=1, z=2

	// Plane X0
	setPlaneForce(mesh.getBoundary()->getPlaneX0(), nodes, 0);

	// Plane Y0
	setPlaneForce(mesh.getBoundary()->getPlaneY0(), nodes, 1);

	// Plane Z0
	setPlaneForce(mesh.getBoundary()->getPlaneZ0(), nodes, 2);
	
	// Plane Xn
	setPlaneForce(mesh.getBoundary()->getPlaneXn(), nodes, 0);

	// Plane Yn
	setPlaneForce(mesh.getBoundary()->getPlaneYn(), nodes, 1);

	// Plane Zn
	setPlaneForce(mesh.getBoundary()->getPlaneZn(), nodes, 2);
}

void Update::contributionNodes(Mesh& mesh, vector<Particle*>* particles){

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i)
	{	
		// update the contribution nodes
		particles->at(i)->updateContributionNodes(mesh);
	}
}
