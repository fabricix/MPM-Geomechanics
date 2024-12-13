/*
 * NodeContact.cpp
 *
 *  Created on: 11 de dez de 2024
 *      Author: Matheus Peres <matheuslperes@tmail.com>
 */

#include "Mesh/NodeContact.h"
#include "Model.h"

NodeContact::~NodeContact() {
	// TODO Auto-generated destructor stub
}

void NodeContact::updateTotalForce(){

    // total force of fluid in mixture
    this->totalForceSlaveBody = this->internalForceSlaveBody + this->externalForceSlaveBody + this->dampingForceSlaveBody;

    // total force of solid in mixture
    Node::totalForce = Node::internalForce + Node::externalForce + Node::dampingForce - this->totalForceSlaveBody;
}

void NodeContact::updateDampingForce() { 

    // update damping force of solid phase
    Node::updateDampingForce();

    // update damping force of fluid phase
    switch(ModelSetup::getDampingType())
    {
    	// local damping force
        case ModelSetup::DampingType::LOCAL:
        {
            double alpha = ModelSetup::getDampingLocal();
            
            Vector3d unbalancedForce = this->internalForceSlaveBody +this->externalForceSlaveBody;

            Vector3d velDirection(0,0,0);

            if(this->velocitySlaveBody.norm()!=0){
                
                velDirection = (this->velocitySlaveBody)/((this->velocitySlaveBody).norm());

                this->dampingForceSlaveBody = - alpha*unbalancedForce.norm()*velDirection;
            }
        }
        break;

        // undamped system
        case ModelSetup::DampingType::UNDAMPED:

        // default case
        default:
            (this->dampingForceSlaveBody).setZero();
    }
}

void NodeContact::integrateMomentum(double dt) { 

    // intetrate momentum of solid phase
    Node::integrateMomentum(dt);

    // integrate momentum of fluid phase
    this->momentumSlaveBody += this->totalForceSlaveBody *dt;
}

void NodeContact::updateVelocity(){ 

    // update node velocity of solid
    Node::updateVelocity();

    // update node velocity of fluid
    if (this->massSlaveBody >0.0)
    {
        this->velocitySlaveBody = this->momentumSlaveBody / this->massSlaveBody;
    }
}
