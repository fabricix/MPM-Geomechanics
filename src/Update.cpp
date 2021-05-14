/*
 * Update.cpp
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Update.h"

Update::Update() {
	// TODO Auto-generated constructor stub

}

Update::~Update() {
	// TODO Auto-generated destructor stub
}

void Update::nodalVelocity(Mesh& mesh){

	// reference to grid nodes
	vector<Node>* gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < gNodes->size(); ++i)
	{
		gNodes->at(i).setVelocity(gNodes->at(i).getMomentum()/gNodes->at(i).getMass());
	}
}

void Update::resetNodalValues(Mesh& mesh)
{
	// reference to grid nodes
	vector<Node>* gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < gNodes->size(); ++i)
	{
		gNodes->at(i).resetValues();
	}
}

void Update::particleDensity(vector<Particle>& particles){

	// For each particle 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		double density = particles.at(i).getDensity();
		double volStrain = particles.at(i).getStrainIncrement().trace();

		// update particle density
		particles.at(i).setDensity(density/(1+volStrain));
	}
}

void Update::particleStress(vector<Particle>& particles){

	// For each particle 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// update particle stress
		particles.at(i).updateStress();
	}
}

void Update::particleVelocity(Mesh& mesh, vector<Particle>& particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// initialize the result vector
		Vector3d velocityRate = Vector3d::Zero();
		
		// For each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// compute the velocity rate contributuion
			velocityRate+=nodes->at(contribution->at(j).getNodeId()).getTotalForce()*contribution->at(j).getWeight()/nodes->at(contribution->at(j).getNodeId()).getMass();
		}

		// update particle velocity
		particles.at(i).setVelocity(particles.at(i).getVelocity()+velocityRate*dt);
	}
}

void Update::particlePosition(Mesh& mesh, vector<Particle>& particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// initialize the result vector
		Vector3d positionRate = Vector3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// compute the position rate contribution
			positionRate+=nodes->at(contribution->at(j).getNodeId()).getMomentum()*contribution->at(j).getWeight()/nodes->at(contribution->at(j).getNodeId()).getMass();
		}

		// update particle position
		particles.at(i).setPosition(particles.at(i).getPosition()+positionRate*dt);
	}
}
