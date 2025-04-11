/*
 * Particle.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include<string>
using std::string;
using std::to_string;

#include "Particle/Particle.h"
#include "Mesh/Mesh.h"

int Particle::totalParticles=0;

Particle::Particle(const Vector3d& position, Material* material, const Vector3d& size){

	// local variables

	double porosity = material!=0?material->getPorosity():0.0;
	
	// configures members
	
	this->active=true;
	this->id=totalParticles;
	this->bodyId=0;

	this->density=material!=0?material->getDensity():0.0;
	this->mass=(size.x()*size.y()*size.z())*this->density*(1.0-porosity);
	this->plasticStrain=0.0;

	this->initialPosition=position;
	this->position=position;
	this->velocity.setZero();
	this->externalForce.setZero();
	this->size=size;

	this->stress.setZero();
	this->strain.setZero();
	this->strainIncrement.setZero();
	this->vorticityIncrement.setZero();
	this->deformationGradient=Matrix3d::Identity();

	this->contributionNodes.clear();
	this->contributionNodes.resize(ModelSetup::getContributionNodesNum());
	
	this->shape=0;
	this->material=material;

	totalParticles++;
}

void Particle::updateContributionNodes(Mesh* mesh)
{
	// verify if the particle in inside the mesh
	if (!mesh->getIsInsideMesh(position)){

		setActive(false);
		return;
	}

	// nodes that the particle contributed
	vector<int> nodesId;
	mesh->getContributionNodes(position,nodesId);
	
	// reference to grid nodes
	vector<Node*>* gNodes = mesh->getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < nodesId.size(); ++i)
	{	
		// set the node id
		contributionNodes.at(i).setNodeId(nodesId.at(i));
		//clear subdomain data
		contributionNodes.at(i).clearSubdomain(i);

		// update the shape functions and gradients
		shape->update(position,gNodes->at(nodesId.at(i))->getCoordinates(),mesh->getCellDimension(),size);

		// get shape function and its derivates
		Vector3d shapeFn = shape->getShape();
		Vector3d derivateFn = shape->getDerivate();

		// set weight
		contributionNodes.at(i).setWeight(shapeFn.x()*shapeFn.y()*shapeFn.z());

		// update gradients
		Vector3d gradient;
		gradient.x() = derivateFn.x()*shapeFn.y()*shapeFn.z();
		gradient.y() = shapeFn.x()*derivateFn.y()*shapeFn.z();
		gradient.z() = shapeFn.x()*shapeFn.y()*derivateFn.z();

		// set gradients
		contributionNodes.at(i).setGradients(gradient);
	}
}

void Particle::updateDensity() {

	// volumetric strain increment
	double volStrainInc = strainIncrement.trace();
	
	// update particle density
	if ((1.0+volStrainInc)!=0.0){

		density = density / (1.0+volStrainInc);
	}
}

void Particle::setMaterial(Material* material) { 

	// set material pointer
	this->material=material;

	// update attributes
	double porosity = material!=0?material->getPorosity():0.0;
	this->density=material!=0?material->getDensity():0.0;
	this->mass=(size.x()*size.y()*size.z())*this->density*(1.0-porosity);
}