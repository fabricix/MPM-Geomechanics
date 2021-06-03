/*
 * Node.cpp
 *
 *  Created on: 3 de jun de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Model.h"
#include "Node.h"

#include "Eigen/Core"
using Eigen::Vector3d;

void Node::updateDampingForce() { 

	ModelSetup::DampingType dampingType = ModelSetup::getDamping();

	switch(dampingType) {

		case ModelSetup::DampingType::LOCAL:
		{
			double alpha = ModelSetup::getDampingLocal();
            Vector3d unbalancedForce = this->internalForce+this->externalForce;
            this->dampingForce = - alpha*unbalancedForce.norm()*((this->velocity)/(this->velocity).norm());
		}
		break;

		case ModelSetup::DampingType::RAYLEIGH:
		case ModelSetup::DampingType::ARTIFICIAL_VISCOSITY:
		case ModelSetup::DampingType::UNDAMPED:
		default:
			(this->dampingForce).setZero();		
	}
}
