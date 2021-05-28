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
	/// \return Node identification
	inline int getNodeId() const { return this->nodeId; }

	/// \brief Return the weight value
	/// \return Nodal weight. Value of the nodal weight evaluated at the particle position
	inline double getWeight() const { return this->weight; }

	/// \brief Return the gradients values
	/// \return Nodal gradients. Nodal gradients in each direction evaluated at the particle position
	inline Vector3d getGradients() const { return this->gradient; }

	/// \brief Configure node identification
	/// \param[in] nodal_id Node identification
	inline void setNodeId(int nodal_id) { this->nodeId=nodal_id; }
	
	/// \brief Configure weight
	/// \param[in] nodal_weight Value of the nodal weight evaluated at the particle position
	inline void setWeight(double nodal_weight) { this->weight=nodal_weight; }
	
	/// \brief Configure gradients
	/// \param[in] nodal_gradients Nodal gradients in each direction evaluated at the particle position
	inline void setGradients(Vector3d nodal_gradients) { this->gradient=nodal_gradients; };

private:
	
	int nodeId; //!< identification of node

	double weight; //!< weight value of a node at a particle

	Vector3d gradient; //!< gradient value of a node at a particle
};

inline Contribution::Contribution()
:nodeId(0),weight(0.0),gradient(0.0,0.0,0.0) {

}

inline Contribution::~Contribution() {
	
}

#endif /* CONTRIBUTION_H_ */