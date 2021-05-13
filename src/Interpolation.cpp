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

//
//
// From particle to node
//
//

void Interpolation::nodalMass(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// get the particle mass
		double pMass = particles.at(i).getMass();

		// For each node in the contribution list 
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// add the weighted quantity in node
			nodes->at(contribution->at(j).getNodeId()).addMass(pMass*contribution->at(j).getWeight());
		}
	}
}

void Interpolation::nodalMomentum(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// get the particle momentum
		Vector3d pMomentum = particles.at(i).getMomentum();

		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// add the weighted quantity in node
			nodes->at(contribution->at(j).getNodeId()).addMomentum(pMomentum*contribution->at(j).getWeight());
		}
	}
}

void Interpolation::nodalInternalForce(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// obtain particle stress
		Matrix3d pStress = particles.at(i).getStress();
		
		// get the particle volume
		double pVolume = particles.at(i).getMass()/particles.at(i).getDensity();

		// For each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get the nodal gradients
			Vector3d gradient=contribution->at(j).getGradients();

			// and compute the internal force
			Vector3d internalForce;
			internalForce.x()=-(pStress(0,0)*gradient.x()+pStress(1,0)*gradient.y()+pStress(2,0)*gradient.z())*pVolume;
			internalForce.y()=-(pStress(0,1)*gradient.x()+pStress(1,1)*gradient.y()+pStress(2,1)*gradient.z())*pVolume;
			internalForce.z()=-(pStress(0,2)*gradient.x()+pStress(1,2)*gradient.y()+pStress(2,2)*gradient.z())*pVolume;

			// add the quantity in node
			nodes->at(contribution->at(j).getNodeId()).addInternalForce(internalForce);
		}
	}
}

void Interpolation::nodalExternalForce(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// get external force at particle
		Vector3d pExtForce = particles.at(i).getExternalForce();

		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// add the weighted quantity in node
			nodes->at(contribution->at(j).getNodeId()).addExternalForce(pExtForce*contribution->at(j).getWeight());
		}
	}
}

//
//
// From node to particle
//
//

void Interpolation::particleStrainIncrement(Mesh& mesh, vector<Particle>& particles,double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// initialize a matrix for strain increment computation
		Matrix3d dstrain = Matrix3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get gradient
			Vector3d dN = contribution->at(j).getGradients();

			// get velocity
			Vector3d v = nodes->at(contribution->at(j).getNodeId()).getVelocity();

			dstrain(0,0) += (dN(0)*v(0)+dN(0)*v(0))*0.5*dt;
			dstrain(0,1) += (dN(1)*v(0)+dN(0)*v(1))*0.5*dt;
			dstrain(0,2) += (dN(2)*v(0)+dN(0)*v(2))*0.5*dt;
			
			dstrain(1,0) += (dN(0)*v(1)+dN(1)*v(0))*0.5*dt;
			dstrain(1,1) += (dN(1)*v(1)+dN(1)*v(1))*0.5*dt;
			dstrain(1,2) += (dN(2)*v(1)+dN(1)*v(2))*0.5*dt;
			
			dstrain(2,0) += (dN(0)*v(2)+dN(2)*v(0))*0.5*dt;
			dstrain(2,1) += (dN(1)*v(2)+dN(2)*v(1))*0.5*dt;
			dstrain(2,2) += (dN(2)*v(2)+dN(2)*v(2))*0.5*dt;
		}

		// add quantity in particle
		particles.at(i).setStrainIncrement(dstrain);
	}
}

void Interpolation::particleVorticityIncrement(Mesh& mesh, vector<Particle>& particles,double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// initialize a matrix for strain increment computation
		Matrix3d dvorticity = Matrix3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get gradient
			Vector3d dN = contribution->at(j).getGradients();

			// get velocity
			Vector3d v = nodes->at(contribution->at(j).getNodeId()).getVelocity();

			dvorticity(0,0) += (dN(0)*v(0)-dN(0)*v(0))*0.5*dt;
			dvorticity(0,1) += (dN(1)*v(0)-dN(0)*v(1))*0.5*dt;
			dvorticity(0,2) += (dN(2)*v(0)-dN(0)*v(2))*0.5*dt;
			
			dvorticity(1,0) += (dN(0)*v(1)-dN(1)*v(0))*0.5*dt;
			dvorticity(1,1) += (dN(1)*v(1)-dN(1)*v(1))*0.5*dt;
			dvorticity(1,2) += (dN(2)*v(1)-dN(1)*v(2))*0.5*dt;
			
			dvorticity(2,0) += (dN(0)*v(2)-dN(2)*v(0))*0.5*dt;
			dvorticity(2,1) += (dN(1)*v(2)-dN(2)*v(1))*0.5*dt;
			dvorticity(2,2) += (dN(2)*v(2)-dN(2)*v(2))*0.5*dt;
		}

		// add quantity in particle
		particles.at(i).setVorticityIncrement(dvorticity);
	}
}

void Interpolation::particleVelocityRate(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// initialize the result vector
		Vector3d velocityRate = Vector3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get weight
			double Nip = contribution->at(j).getWeight();

			// get total nodal force
			Vector3d totalForce = nodes->at(contribution->at(j).getNodeId()).getTotalForce();

			// get nodal mass
			double nodalMass = nodes->at(contribution->at(j).getNodeId()).getMass();

			// compute the contribution
			velocityRate+=totalForce*Nip/nodalMass;
		}

		// add quantity in particle
		particles.at(i).setVelocityRate(velocityRate);
	}
}

void Interpolation::particlePositionRate(Mesh& mesh, vector<Particle>& particles){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh.getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// get the nodes and weights that this particle contributes.
		vector<Contribution>* contribution = particles.at(i).getContributionNodes();

		// initialize the result vector
		Vector3d velocityRate = Vector3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j)
		{	
			// get weight
			double Nip = contribution->at(j).getWeight();

			// get total nodal force
			Vector3d nodalMomentum = nodes->at(contribution->at(j).getNodeId()).getMomentum();

			// get nodal mass
			double nodalMass = nodes->at(contribution->at(j).getNodeId()).getMass();

			// compute the contribution
			velocityRate+=nodalMomentum*Nip/nodalMass;
		}

		// add quantity in particle
		particles.at(i).setPositionRate(velocityRate);
	}
}
