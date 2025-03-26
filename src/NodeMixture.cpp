// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Mesh/NodeMixture.h"
#include "Model.h"

NodeMixture::~NodeMixture() {
	// TODO Auto-generated destructor stub
}

void NodeMixture::updateTotalForce(){

    // total force of fluid in mixture
    this->totalForceFluid = this->internalForceFluid + this->externalForceFluid + this->dampingForceFluid;

    // total force of solid in mixture
    Node::totalForce = Node::internalForce + Node::externalForce + Node::dampingForce - this->totalForceFluid;
}

void NodeMixture::updateDampingForce() { 

    // update damping force of solid phase
    Node::updateDampingForce();

    // update damping force of fluid phase
    switch(ModelSetup::getDampingType())
    {
    	// local damping force
        case ModelSetup::DampingType::LOCAL:
        {
            double alpha = ModelSetup::getDampingLocal();
            
            Vector3d unbalancedForce = this->internalForceFluid+this->externalForceFluid;

            Vector3d velDirection(0,0,0);

            if(this->velocityFluid.norm()!=0){
                
                velDirection = (this->velocityFluid)/((this->velocityFluid).norm());

                this->dampingForceFluid = - alpha*unbalancedForce.norm()*velDirection;
            }
        }
        break;

        // undamped system
        case ModelSetup::DampingType::UNDAMPED:

        // default case
        default:
            (this->dampingForceFluid).setZero();     
    }
}

void NodeMixture::integrateMomentum(double dt) { 

    // intetrate momentum of solid phase
    Node::integrateMomentum(dt);

    // integrate momentum of fluid phase
    this->momentumFluid += this->totalForceFluid*dt;
}

void NodeMixture::updateVelocity(){ 

    // update node velocity of solid
    Node::updateVelocity();

    // update node velocity of fluid
    if (this->massFluid>0.0)
    {
        this->velocityFluid = this->momentumFluid / this->massFluid; 
    }
}
