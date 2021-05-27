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

	/// \brief Default constructor
	///
	Contribution();
	
	/// \brief Default destructor
	///
	virtual ~Contribution();

	/// \brief Return the node identification
	/// \param[out] node_id Node identification
	int getNodeId() const;
	
	/// \brief Return the weight value
	/// \param[out] nodal_weight Value of the nodal weight evaluated at the particle position
	double getWeight() const;
	
	/// \brief Return the gradients values
	/// \param[out] nodal_gradients Nodal gradients in each direction evaluated at the particle position
	Vector3d getGradients() const;

	/// \brief Configure node identification
	/// \param[int] nodal_id Node identification
	void setNodeId(int);
	
	/// \brief Configure weight
	/// \param[int] nodal_weight Value of the nodal weight evaluated at the particle position
	void setWeight(double);
	
	/// \brief Configure gradients
	/// \param[int] nodal_gradients Nodal gradients in each direction evaluated at the particle position
	void setGradients(Vector3d);

private:
	
	int nodeId; //!< identification of node

	double weight; //!< weight value of a node at a particle

	Vector3d gradient; //!< gradient value of a node at a particle
};

#endif /* CONTRIBUTION_H_ */
