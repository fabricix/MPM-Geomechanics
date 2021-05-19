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
		if(gNodes->at(i).getActive()&&gNodes->at(i).getMass()!=0.0){
		
			gNodes->at(i).setVelocity(gNodes->at(i).getMomentum()/gNodes->at(i).getMass());
		}
	}
}

void Update::nodalTotalForce(Mesh& mesh){

	// reference to grid nodes
	vector<Node>* gNodes = mesh.getNodes();

	// update total force in nodes
	for (size_t i = 0; i < gNodes->size(); ++i)
	{
		if(gNodes->at(i).getActive()){
		
			gNodes->at(i).setTotalForce(gNodes->at(i).getInternalForce()+gNodes->at(i).getExternalForce());
		}
	}
}

void Update::resetNodalValues(Mesh& mesh){

	// reference to grid nodes
	vector<Node>* gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < gNodes->size(); ++i)
	{
		if(gNodes->at(i).getActive()){
			
			gNodes->at(i).resetValues();
		}
	}
}

void Update::particleDensity(vector<Particle*>& particles){

	// For each particle 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		double volStrain = particles.at(i)->getStrainIncrement().trace();
		
		// update particle density
		if ((1+volStrain)!=0){

			particles.at(i)->setDensity(particles.at(i)->getDensity()/(1+volStrain));
		}
	}
}

void Update::particleStress(vector<Particle*>& particles){

	// For each particle 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// update particle stress
		particles.at(i)->updateStress();
	}
}

void Update::particleVelocity(Mesh& mesh, vector<Particle*>& particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i)->getContributionNodes();

		// initialize the result vector
		Vector3d velocityRate = Vector3d::Zero();
		
		// For each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// compute the velocity rate contributuion
			if (nodes->at(contribution->at(j).getNodeId()).getMass()!=0.0)
			{
				velocityRate+=nodes->at(contribution->at(j).getNodeId()).getTotalForce()*contribution->at(j).getWeight()/nodes->at(contribution->at(j).getNodeId()).getMass();
			}
		}

		// update particle velocity
		particles.at(i)->setVelocity(particles.at(i)->getVelocity()+velocityRate*dt);
	}
}

void Update::particlePosition(Mesh& mesh, vector<Particle*>& particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i)->getContributionNodes();

		// initialize the result vector
		Vector3d positionRate = Vector3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// compute the position rate contribution
			if (nodes->at(contribution->at(j).getNodeId()).getMass()!=0.0)
			{
				positionRate+=nodes->at(contribution->at(j).getNodeId()).getMomentum()*contribution->at(j).getWeight()/nodes->at(contribution->at(j).getNodeId()).getMass();
			}
		}

		// update particle position
		particles.at(i)->setPosition(particles.at(i)->getPosition()+positionRate*dt);
	}
}

void Update::setPlaneMomentum(const Boundary::planeBoundary* plane, vector<Node>* nodes, int dir){

	for (size_t i = 0; i < plane->nodes.size(); ++i)
	{
		if (nodes->at(plane->nodes.at(i)).getActive()){

			switch(plane->type) {

				case Boundary::BoundaryType::FIXED:
					nodes->at(plane->nodes.at(i)).setMomentum(Vector3d::Zero());
				break;
			
				case Boundary::BoundaryType::SLIDING:
					
					Vector3d momentum = nodes->at(plane->nodes.at(i)).getMomentum();
					
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

					nodes->at(plane->nodes.at(i)).setMomentum(momentum);

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
		if (nodes->at(plane->nodes.at(i)).getActive()){
			
			switch(plane->type) {

				case Boundary::BoundaryType::FIXED:
					nodes->at(plane->nodes.at(i)).setTotalForce(Vector3d::Zero());
				break;
			
				case Boundary::BoundaryType::SLIDING:
					
					Vector3d force = nodes->at(plane->nodes.at(i)).getTotalForce();
					
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
					nodes->at(plane->nodes.at(i)).setTotalForce(force);
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

void Update::contributionNodes(Mesh& mesh, vector<Particle*>& particles){

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{	
		// update the contribution nodes
		particles.at(i)->updateContributionNodes(mesh);
	}
}
