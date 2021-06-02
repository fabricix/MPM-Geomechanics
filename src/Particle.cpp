/*
 * Particle.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include<string>
using std::string;
using std::to_string;

#include "Particle.h"
#include "Mesh.h"

int Particle::totalParticles=0;

Particle::Particle() {

	initializeValues();

	totalParticles++;

	setId(totalParticles);
}

Particle::Particle(const Vector3d& position, Material* material, const Vector3d& size){

	initializeValues();

	setPosition(position);
	
	setInitialPosition(position);
	
	setMaterial(material);

	setDensity(material!=0?material->getDensity():0.0);

	setSize(size);

	setMass(size.x()*size.y()*size.z()*getDensity());

	setId(totalParticles);

	totalParticles++;
}

Particle::Particle(const Vector3d& position, Material* material) {

	initializeValues();

	setPosition(position);
	
	setInitialPosition(position);

	setMaterial(material);

	setDensity(material!=0?material->getDensity():0.0);

	setId(totalParticles);

	totalParticles++;
}

Particle::Particle(const Vector3d& position) {

	initializeValues();

	setPosition(position);

	setInitialPosition(position);

	setId(totalParticles);

	totalParticles++;
}

void Particle::updateContributionNodes(Mesh& mesh)
{
	// nodes that the particle contributed
	const vector<int> nodesId = mesh.getContributionNodes(position);
	
	// reference to grid nodes
	vector<Node> * gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < nodesId.size(); ++i)
	{	
		// set the node id
		contributionNodes.at(i).setNodeId(nodesId.at(i));
		
		// active the node
		gNodes->at(nodesId.at(i)).setActive(true);
	
		// update the shape functions and gradients
		shape->update(position,gNodes->at(nodesId.at(i)).getCoordinates(),mesh.getCellDimension(),size);

		// get shape function and its derivates
		Vector3d shapeFn = shape->getShape();
		Vector3d derivateFn = shape->getDerivate();

		// update weight
		contributionNodes.at(i).setWeight(shapeFn.x()*shapeFn.y()*shapeFn.z());

		// update gradients
		Vector3d gradient;
		gradient.x() = derivateFn.x()*shapeFn.y()*shapeFn.z();
		gradient.y() = shapeFn.x()*derivateFn.y()*shapeFn.z();
		gradient.z() = shapeFn.x()*shapeFn.y()*derivateFn.z();

		contributionNodes.at(i).setGradients(gradient);
	}
}

//
// private methods
// 

void Particle::initializeValues(){

	id=0;
	bodyId=0;

	mass=0.0;
	density=0.0;
	volume=0.0;
	initialVolume=0.0;
	plasticStrain=0.0;
	porosity=0.0;

	initialPosition.setZero();
	position.setZero();
	velocity.setZero();
	externalForce.setZero();
	size.setZero();

	stress.setZero();
	strain.setZero();
	strainIncrement.setZero();
	vorticityIncrement.setZero();
	deformationGradientIncrement.setZero();
	velocityGradient.setZero();

	contributionNodes.clear();
	contributionNodes.resize(ModelSetup::getContributionNodesNum());
	
	shape=0;
	material=0;
}

void Particle::updateDensity() {

	// volumetric strain increment
	double volStrainInc = strainIncrement.trace();
	
	// update particle density
	if ((1.0+volStrainInc)!=0.0){

		density = density / (1.0+volStrainInc);
	}
}