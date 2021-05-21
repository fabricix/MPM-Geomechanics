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

Vector3d Node::getTotalForce() {

    return totalForce;
}

Vector3d Node::getCoordinates(){

    return coordinates;
}

Vector3d Node::getMomentum(){

    return momentum;
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

Vector3d Node::getVelocity(){

    return velocity;
}

Vector3d Node::getInternalForce(){

    return internalForce;
}
    
Vector3d Node::getExternalForce(){

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
