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
	void setVelocity(Vector3d); //!< configures the velocity
	void setMomentum(Vector3d); //!< configures the momentum
	void setTotalForce(Vector3d); //!< configures the total nodal force

	int getId(); //!< returns the identification
	bool getActive(); //!< returns the active state
	double getMass(); //!< returns the mass
	Vector3d getCoordinates(); //!< returns the coordinates
	Vector3d getMomentum(); //!< returns the momentum
	Vector3d getInternalForce(); //!< returns the internal force
	Vector3d getExternalForce(); //!< returns the external force
	Vector3d getTotalForce(); //!< returns the total nodal force
	Vector3d getVelocity(); //!< returns the velocity

	void addMass(double); //!< adds mass to the nodal mass
	void addMomentum(Vector3d); //!< adds momentum to the nodal momentum
	void addInternalForce(Vector3d); //!< adds internal force to the nodal internal force
	void addExternalForce(Vector3d); //!< adds external force to the nodal external force

	void resetValues(); //!< delete all values sored in node mesh

private:

	bool active; //!< is active node
	int id; //!< nodal identificator
	double mass; //!< nodal mass
	
	Vector3d coordinates; //!< node coordinates
	Vector3d momentum; //!< nodal momentum
	Vector3d velocity; //!< nodal velocity
	Vector3d acceleration; //!< nodal acceleration
	Vector3d externalForce; //!<  total external force
	Vector3d internalForce; //!< nodal internal force
	Vector3d totalForce; //!< nodal total force
};

#endif /* NODE_H_ */
