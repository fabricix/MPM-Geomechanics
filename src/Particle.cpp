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
	
	this->silentParticle = false;

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

Vector3d Particle::getSilentForce(Vector3d cell_dimension)
{	
	// check if silent particle
	if (!this->silentParticle) return Vector3d(0.0, 0.0, 0.0);

	// get sound waves
	double p_wave_velocity = this->material->getCompressionalWaveSpeed();
	double s_wave_velocity = this->material->getShearWaveSpeed();
	
	// initialize normal and tangential components
	double stress_normal(0.0), tangencial_1(0.0), tangencial_2(0.0);

	// silent in x direction
	if (this->silentDirection.x())
	{
		stress_normal = - this->mass * p_wave_velocity * this->velocity.x() / cell_dimension.x();
		tangencial_1  = - this->mass * s_wave_velocity * this->velocity.y() / cell_dimension.x() ;
		tangencial_2  = - this->mass * s_wave_velocity * this->velocity.z() / cell_dimension.x();
		return Vector3d(stress_normal, tangencial_1, tangencial_2);
	}
	// silent force in y direction
	if (this->silentDirection.y())
	{
		tangencial_1  = - this->mass * s_wave_velocity * this->velocity.x() / cell_dimension.y();
		stress_normal = - this->mass * p_wave_velocity * this->velocity.y() / cell_dimension.y();
		tangencial_2  = - this->mass * s_wave_velocity * this->velocity.z() / cell_dimension.y();
		return Vector3d(tangencial_1, stress_normal, tangencial_2);
	}
	// silent force in z direction
	if (this->silentDirection.z())
	{
		tangencial_1  = - this->mass * s_wave_velocity * this->velocity.x() / cell_dimension.z();
		tangencial_2  = - this->mass * s_wave_velocity * this->velocity.y() / cell_dimension.z();
		stress_normal = - this->mass * p_wave_velocity * this->velocity.z() / cell_dimension.z();
		return Vector3d(tangencial_1, tangencial_2, stress_normal);
	}
	// return default value
	return Vector3d(0.0, 0.0, 0.0);
}

void Particle::setMaterial(Material* material) { 

	// set material pointer
	this->material=material;

	// update attributes
	double porosity = material!=0?material->getPorosity():0.0;
	this->density=material!=0?material->getDensity():0.0;
	this->mass=(size.x()*size.y()*size.z())*this->density*(1.0-porosity);
}