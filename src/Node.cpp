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

    shapeType=InterpolationFunctionType::GIMP;
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

const Vector3d & Node::getCoordinates(){

    return coordinates;
}

int Node::getId(){
    return id;
}

bool Node::getActive(){

	return active;
}

