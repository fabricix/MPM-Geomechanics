/*
 * Node.cpp
 *
 *  Created on: 14 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Node.h"

Node::~Node() {
    // TODO Auto-generated destructor stub
}

Node::Node() {

    active=false;

    id=0;

    mass=0.0;

    coordinates.setZero();
    momentum.setZero();
    force.setZero();
    acceleration.setZero();
}

//
// set methods
//

void Node::setId(int nid){
    
    id=nid;
}

void Node::setCoordinates(Vector3d nc){
    
    coordinates=nc;
}

void Node::setActive(bool a){

    active=a;
}

//
// get methods
//

Vector3d Node::getCoordinates(){

    return coordinates;
}

int Node::getId(){
    return id;
}

bool Node::getActive(){

	return active;
}

double Node::getMass()
{
    return mass;
}

//
// add methods
//

void Node::addMass(double dm){

	mass+=dm;
}

void Node::addMomentum(Vector3d dp){

    momentum+=dp;
}

void Node::addInternalForce(Vector3d fint){

    internalForce+=fint;
}
