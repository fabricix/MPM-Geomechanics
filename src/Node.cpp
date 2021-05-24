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
    velocity.setZero();
    acceleration.setZero();
    externalForce.setZero();
    internalForce.setZero();
    totalForce.setZero();
}

void Node::resetValues()
{
    active=false;
    mass=0.0;

    momentum.setZero();
    velocity.setZero();
    acceleration.setZero();
    externalForce.setZero();
    internalForce.setZero();
    totalForce.setZero();
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

void Node::setVelocity(Vector3d v){

    velocity=v;
}

void Node::setMomentum(Vector3d p){

    momentum=p;
}

void Node::setTotalForce(Vector3d f){

    totalForce=f;
}

//
// get methods
//

Vector3d Node::getTotalForce() const {

    return totalForce;
}

Vector3d Node::getCoordinates() const {

    return coordinates;
}

Vector3d Node::getMomentum() const {

    return momentum;
}

int Node::getId() const {

    return id;
}

bool Node::getActive() const {

	return active;
}

double Node::getMass() const {

    return mass;
}

Vector3d Node::getVelocity() const {

    return velocity;
}

Vector3d Node::getInternalForce() const {

    return internalForce;
}
    
Vector3d Node::getExternalForce() const {

    return externalForce;
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

void Node::addInternalForce(Vector3d dfint){

    internalForce+=dfint;
}

void Node::addExternalForce(Vector3d dfext){

    externalForce+=dfext;
}
