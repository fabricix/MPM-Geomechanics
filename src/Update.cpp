/*
 * Update.cpp
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Update.h"
#include "Particle.h"

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