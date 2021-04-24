/*
 * Contribution.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef CONTRIBUTION_H_
#define CONTRIBUTION_H_

#include <vector>
using std::vector;

class Contribution {
public:
	Contribution();
	virtual ~Contribution();
	//updateWeightGradients();

	int nodeId; //!< id of the contribuited node
	double weight; //!< weight value of a node at a particle
	double gradientX; //!< x gradient value of a node at a particle
	double gradientY; //!< y gradient value of a node at a particle
	double gradientZ; //!< z gradient value of a node at a particle
};

#endif /* CONTRIBUTION_H_ */
