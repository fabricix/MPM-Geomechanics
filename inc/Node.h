/*
 * Node.h
 *
 *  Created on: 14 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef NODE_H_
#define NODE_H_

#include "../inc/Eigen/Core"
using Eigen::Vector3d;

/// \class Node
/// \brief Represents a mesh node.
class Node {

public:

	virtual ~Node();
	Node();

	// set methods
	void setId(int);
	void setActive(bool);
	void setCoordinates(Vector3d);

	// get methods
	int getId();
	bool getActive();
	Vector3d getCoordinates();

	void addMass(double);

private:

	bool active; //!< is active node
	int id; //!< nodal identificator
	double mass; //!< nodal mass
	
	Vector3d coordinates; //!< node coordinates
	Vector3d momentum; //!< nodal momentum
	Vector3d force; //!<  nodal total force
	Vector3d acceleration; //!< nodal acceleration
};

#endif /* NODE_H_ */
