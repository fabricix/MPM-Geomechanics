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

	setId(totalParticles);

	totalParticles++;
}

Particle::Particle(Vector3d position, Material* material) {

	initializeValues();
	
	setId(totalParticles);

	setPosition(position);
	
	setInitialPosition(position);
	
	setMaterial(material);

	totalParticles++;
}

Particle::Particle(Vector3d position) {

	initializeValues();
	
	setId(totalParticles);

	setPosition(position);
	
	setInitialPosition(position);

	totalParticles++;
}

Particle::~Particle() {
	
	totalParticles--;
}

//
// get methods
//

int Particle::getId()
{
	return id;
}

double Particle::getMass()
{
	return mass;
}

Vector3d Particle::getPosition()
{
	return position;
}

vector<Contribution>* Particle::getContributionNodes(){

	return &contributionNodes;
}

int Particle::getTotalParticles()
{
	return totalParticles;
}

Vector3d Particle::getMomentum(){

	return momentum;
}

Matrix3d Particle::getStress(){

	return stress;
}

double Particle::getDensity(){

	return material->getDensity();
}

int Particle::getMaterialId(){

	return material->getId();
}

Vector3d Particle::getExternalForce()
{
	return externalForce;
}

//
// set methods
//

void Particle::setSize(Vector3d pSize){

	size=pSize;
}

void Particle::setMass(double m)
{
	mass=m;
}

void Particle::setPosition(Vector3d x)
{
	position=x;
}

void Particle::setInitialPosition(Vector3d xo)
{
	initialPosition=xo;
}

void Particle::setId(int i)
{
	id=i;
}

void Particle::setMaterial(Material* mat)
{
	material=mat;
}

//
// public methods
//

void Particle::updateContributionNodes(Mesh & mesh)
{
	// nodes that the particle contributed
	const vector<int> nodesId = mesh.getContributionNodes(position);
	
	// reference to grid nodes
	const vector<Node> * gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < nodesId.size(); ++i)
	{	
		// set the node id
		contributionNodes.at(i).setNodeId(nodesId.at(i));
		
		// get the node
		Node iNode = gNodes->at(nodesId.at(i));

		// update the shape functions and gradients
		shape.updateGimp(position,iNode.getCoordinates(),mesh.getCellDimension(), size);

		// update weight
		contributionNodes.at(i).setWeight(shape.Sx*shape.Sy*shape.Sz);

		// update gradients
		Vector3d gradients;
		gradients.x()=shape.Gx*shape.Sy*shape.Sz;
		gradients.y()=shape.Sx*shape.Gy*shape.Sz;
		gradients.z()=shape.Sx*shape.Sy*shape.Gz;

		contributionNodes.at(i).setGradients(gradients);
	}
}

//
// private methods
// 

void Particle::initializeValues(){

	id=0;
	cellId=0;
	bodyId=0;

	mass=0.0;
	density=0.0;
	volume=0.0;
	plasticStrain=0.0;
	initialVolume=0.0;
	porosity=0.0;

	position.setZero();
	initialPosition.setZero();
	velocity.setZero();
	momentum.setZero();
	externalForce.setZero();
	size.setZero();

	stress.setZero();
	strain.setZero();
	deformationGradient.setZero();
	deformationGradientIncrement.setZero();
	velocityGradient.setZero();

	contributionNodes.clear();
	contributionNodes.resize(ModelSetup::getContributionNodesNum());
	material=NULL;
}
