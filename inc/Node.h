/*
 * Node.h
 *
 *  Created on: 14 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef NODE_H_
#define NODE_H_

#include "Eigen/Core"
using Eigen::Vector3d;

/// \class Node
/// \brief Represents a mesh node.
class Node {

public:
	
	virtual ~Node(); //!< default destructor
	Node(); //!< default constructor

	void setId(int); //!< configures the identification
	void setActive(bool); //!< configures the active state
	void setCoordinates(Vector3d); //!< configures the coordinates

	int getId(); //!< returns the identification
	bool getActive(); //!< returns the active state
	Vector3d getCoordinates(); //!< returns the coordinates
	double getMass(); //!< returns the mass

	void addMass(double); //!< adds mass to the nodal mass
	void addMomentum(Vector3d); //!< adds momentum to the nodal momentum
	void addInternalForce(Vector3d); //!< adds internal force to the nodal internal force
	void addExternalForce(Vector3d); //!< adds external force to the nodal external force

private:

	bool active; //!< is active node
	int id; //!< nodal identificator
	double mass; //!< nodal mass
	
	Vector3d coordinates; //!< node coordinates
	Vector3d momentum; //!< nodal momentum
	Vector3d acceleration; //!< nodal acceleration
	Vector3d externalForce; //!<  total external force
	Vector3d internalForce; //!< nodal internal force
};

#endif /* NODE_H_ */
