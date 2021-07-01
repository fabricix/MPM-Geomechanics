/*
 * Node.cpp
 *
 *  Created on: 3 de jun de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Model.h"
#include "Mesh/Node.h"

#include "Eigen/Core"
using Eigen::Vector3d;

void Node::updateDampingForce() { 

	switch(ModelSetup::getDampingType()) {

		case ModelSetup::DampingType::LOCAL:
		{
			double alpha = ModelSetup::getDampingLocal();
            
            Vector3d unbalancedForce = this->internalForce+this->externalForce;

            Vector3d velDirection(0,0,0);

            if(this->velocity.norm()!=0){
            	
            	velDirection = (this->velocity)/((this->velocity).norm());

            	this->dampingForce = - alpha*unbalancedForce.norm()*velDirection;
            }
		}
		break;

		case ModelSetup::DampingType::RAYLEIGH:
		case ModelSetup::DampingType::ARTIFICIAL_VISCOSITY:
		case ModelSetup::DampingType::UNDAMPED:
		default:
			(this->dampingForce).setZero();		
	}
}
