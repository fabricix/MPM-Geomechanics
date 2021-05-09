/*
 * Contribution.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef CONTRIBUTION_H_
#define CONTRIBUTION_H_

#include "Eigen/Core"
using Eigen::Vector3d;

/// \class Contribution
/// \brief Represents the node that the particle contributes.
class Contribution {

public:
	Contribution(); //!< default constructor
	virtual ~Contribution(); //!< default destructor

	int getNodeId(); //!< returns the node identification
	double getWeight(); //!< returns the weight value
	Vector3d getGradients(); //!< returns the gradients values

	void setNodeId(int); //!< configures node identification
	void setWeight(double); //!< configures weight
	void setGradients(Vector3d); //!< configures gradients


private:
	int nodeId; //!< identification of node
	double weight; //!< weight value of a node at a particle
	Vector3d gradient; //!< gradient value of a node at a particle
};

#endif /* CONTRIBUTION_H_ */
