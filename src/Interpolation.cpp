/*
 * Interpolation.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Interpolation.h"
#include "Contribution.h"
#include "Particle.h"

//
// From particle to node
//

void Interpolation::nodalMass(Mesh* mesh, vector<Particle*>* particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh->getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i){

		// get the nodes and weights that this particle contributes.
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// get the particle mass
		const double pMass = particles->at(i)->getMass();

		// For each node in the contribution list 
		for (size_t j = 0; j < contribution->size(); ++j){

			// contribution structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// add the weighted quantity in node
			nodeI.addMass(pMass*contribI.getWeight());
		}
	}
}

void Interpolation::nodalMomentum(Mesh* mesh, vector<Particle*>* particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh->getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i){

		// get the nodes and weights that this particle contributes.
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// get particle velocity
		const Vector3d pVelocity = particles->at(i)->getVelocity();

		// get particle mass
		const double pMass = particles->at(i)->getMass();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j){

			// contribution structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// add the weighted quantity in node
			nodeI.addMomentum(pMass*pVelocity*contribI.getWeight());
		}
	}
}

void Interpolation::nodalInternalForce(Mesh* mesh, vector<Particle*>* particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh->getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i){

		// get the nodes and weights that this particle contributes.
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// obtain particle stress
		const Matrix3d pStress = particles->at(i)->getStress();
		
		// get the particle volume
		const double pVolume = particles->at(i)->getMass()/particles->at(i)->getDensity();

		// For each node in the contribution list
		for (size_t j = 0; j < contribution->size(); ++j){

			// contribution structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// get the nodal gradients
			const Vector3d gradient = contribI.getGradients();

			// and compute the internal force
			Vector3d internalForce = Vector3d::Zero();
			internalForce.x()=-(pStress(0,0)*gradient(0)+pStress(1,0)*gradient(1)+pStress(2,0)*gradient(2))*pVolume;
			internalForce.y()=-(pStress(0,1)*gradient(0)+pStress(1,1)*gradient(1)+pStress(2,1)*gradient(2))*pVolume;
			internalForce.z()=-(pStress(0,2)*gradient(0)+pStress(1,2)*gradient(1)+pStress(2,2)*gradient(2))*pVolume;

			// add the quantity in node
			nodeI.addInternalForce(internalForce);
		}
	}
}

void Interpolation::nodalExternalForce(Mesh* mesh, vector<Particle*>* particles){

	// Get the mesh nodes pointer.
	vector<Node>* nodes = mesh->getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i){

		// get the nodes and weights that this particle contributes.
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// get external force at particle
		const Vector3d pExtForce = particles->at(i)->getExternalForce();

		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j){

			// contribution structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// add the weighted quantity in node
			nodeI.addExternalForce(pExtForce*contribI.getWeight());
		}
	}
}

//
// From node to particle
//

void Interpolation::particleStrainIncrement(Mesh* mesh, vector<Particle*>* particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh->getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i){

		// get the nodes and weights that this particle contributes.
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// initialize a matrix for strain increment computation
		Matrix3d dstrain = Matrix3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j){

			// contribution structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// get gradient
			const Vector3d dN = contribI.getGradients();

			// get velocity
			const Vector3d v = nodeI.getVelocity();

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

		// set quantity in particle
		particles->at(i)->setStrainIncrement(dstrain);
	}
}

void Interpolation::particleVorticityIncrement(Mesh* mesh, vector<Particle*>* particles, double dt){

	// Get the mesh nodes pointer
	vector<Node>* nodes = mesh->getNodes();

	// For each particle, 
	for (size_t i = 0; i < particles->size(); ++i){

		// get the nodes and weights that this particle contributes.
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// initialize a matrix for strain increment computation
		Matrix3d dvorticity = Matrix3d::Zero();
		
		// For each node in the contribution list,
		for (size_t j = 0; j < contribution->size(); ++j){

			// contribution structure
			const Contribution contribI = contribution->at(j);

			// contributing node
			Node& nodeI = nodes->at(contribI.getNodeId());

			// get gradient
			const Vector3d dN = contribI.getGradients();

			// get velocity
			const Vector3d v = nodeI.getVelocity();

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
		particles->at(i)->setVorticityIncrement(dvorticity);
	}
}