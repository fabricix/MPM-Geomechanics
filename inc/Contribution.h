/*
 * Contribution.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef CONTRIBUTION_H_
#define CONTRIBUTION_H_

#include "../inc/Eigen/Core"
using Eigen::Vector3d;

/// \class Contribution
/// \brief Represents the node that una particle contributes.
class Contribution {

public:
	Contribution();
	virtual ~Contribution();

	int getNodeId();
	double getWeight();
	Vector3d getGradients();

	void setNodeId(int);
	void setWeight(double);
	void setGradients(Vector3d);


private:
	int nodeId; //!< id of the contribuited node
	double weight; //!< weight value of a node at a particle
	Vector3d gradient; //!< gradient value of a node at a particle
};

#endif /* CONTRIBUTION_H_ */
