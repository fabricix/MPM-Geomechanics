/*
 * Interpolation.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Interpolation.h"
#include "Contribution.h"
#include "Particle.h"

Interpolation::Interpolation() {
	// TODO Auto-generated constructor stub

}

Interpolation::~Interpolation() {
	// TODO Auto-generated destructor stub
}

void Interpolation::interpolateMassToNodes(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// In each node in the contribution list 
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// compute the mass
			double mass = particles.at(i).getMass()*contribution->at(j).getWeight();
			
			// and add the weighted quantity in node
			nodes->at(contribution->at(j).getNodeId()).addMass(mass);
		}
	}
}

void Interpolation::interpolateMomentumToNodes(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// In each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// calculate the weighted momentum
			Vector3d momentum = particles.at(i).getMomentum()*contribution->at(j).getWeight();

			// and add the weighted quantity in node
			nodes->at(contribution->at(j).getNodeId()).addMomentum(momentum);
		}
	}
}

void Interpolation::interpolateInternalForceToNodes(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// particle stress
		Matrix3d pStress=particles.at(i).getStress();
		
		// particle volume
		double pVolume = particles.at(i).getMass()/particles.at(i).getDensity();

		// In each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// node gradients
			Vector3d gradient=contribution->at(j).getGradients();

			// internal force
			Vector3d internalForce;
			internalForce.x()=-(pStress(0,0)*gradient.x()+pStress(1,0)*gradient.y()+pStress(2,0)*gradient.z())*pVolume;
			internalForce.y()=-(pStress(0,1)*gradient.x()+pStress(1,1)*gradient.y()+pStress(2,1)*gradient.z())*pVolume;
			internalForce.z()=-(pStress(0,2)*gradient.x()+pStress(1,2)*gradient.y()+pStress(2,2)*gradient.z())*pVolume;

			// add quantity in node
			nodes->at(contribution->at(j).getNodeId()).addInternalForce(internalForce);
		}
	}
}
