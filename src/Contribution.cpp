/*
 * Contribution.cpp
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Contribution.h"

Contribution::Contribution()
:nodeId(0),weight(0.0),gradient(0.0,0.0,0.0) {

}

Contribution::~Contribution() {
	// TODO Auto-generated destructor stub
}

int Contribution::getNodeId(){

	return nodeId;
}

double Contribution::getWeight(){

	return weight;
}
Vector3d Contribution::getGradients(){

	return gradient;
}

void Contribution::setNodeId(int nid){

	nodeId=nid;
}

void Contribution::setWeight(double w){

	weight=w;
}

void Contribution::setGradients(Vector3d grad){

	gradient=grad;
}
