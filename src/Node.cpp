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