/*
 * Particle.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Particle.h"
#include "Mesh.h"

Particle::Particle() {

	initializeValues();
}

Particle::Particle(Vector3d particlePosition) {

	initializeValues();
	
	position=particlePosition;
	initialPosition=particlePosition;
}

Particle::~Particle() {
	// TODO Auto-generated destructor stub
}

//
// publics methods
//

int Particle::getId()
{
	return id;
}

Vector3d Particle::getPosition()
{
	return position;
}

const vector<Contribution>& Particle::getContributionNodes(){

	return contributionNodes;
}

void Particle::updateContributionNodes(Mesh & mesh)
{
	// nodes that the particle contributed
	const vector<int> nodesId = mesh.getContributionNodes(position);
	
	// reference to grid nodes
	const vector<Node> & gNodes = mesh.getNodes();

	// update the shape function for all nodes that this particle contributes
	for (size_t i = 0; i < nodesId.size(); ++i)
	{	
		// set the node id
		contributionNodes.at(i).nodeId=nodesId.at(i);
		
		// get the node
		Node iNode = gNodes.at(nodesId.at(i));

		// update the shape functions and gradients
		shape.updateGimp(position,iNode.getCoordinates(),mesh.getCellDimension(), size);

		// update contribution
		contributionNodes.at(i).weight   =shape.Sx*shape.Sy*shape.Sz;
		contributionNodes.at(i).gradientX=shape.Gx*shape.Sy*shape.Sz;
		contributionNodes.at(i).gradientY=shape.Sx*shape.Gy*shape.Sz;
		contributionNodes.at(i).gradientZ=shape.Sx*shape.Sy*shape.Gz;
	}
}

void Particle::setSize(Vector3d pSize){

	size=pSize;
}
//
// private methods
// 

void Particle::initializeValues(){

	id=0;
	materialId=0;
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
}
