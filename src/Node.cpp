// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Model.h"
#include "Mesh/Node.h"

#include "Eigen/Core"
using Eigen::Vector3d;

void Node::updateDampingForce() { 

	switch(ModelSetup::getDampingType())
	{
		// local damping force
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

		// undamped system
		case ModelSetup::DampingType::UNDAMPED:

		// default case
		default:
			(this->dampingForce).setZero();		
	}
}
