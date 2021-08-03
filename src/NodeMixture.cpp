/*
 * NodeMixture.cpp
 *
 *  Created on: 23 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Mesh/NodeMixture.h"
#include "Model.h"

NodeMixture::~NodeMixture() {
	// TODO Auto-generated destructor stub
}

void NodeMixture::updateTotalForce()
{
    // total force of fluid phase
    this->totalForceFluid = this->internalForceFluid + this->externalForceFluid + this->dampingForceFluid;

    // total force of solid phase
    Node::totalForce = Node::internalForce + Node::externalForce + Node::dampingForce + this->totalForceFluid;
}

void NodeMixture::updateDampingForce() { 

    switch(ModelSetup::getDampingType()) {

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

        case ModelSetup::DampingType::RAYLEIGH:
        case ModelSetup::DampingType::ARTIFICIAL_VISCOSITY:
        case ModelSetup::DampingType::UNDAMPED:
        default:
            (this->dampingForceFluid).setZero();     
    }
}