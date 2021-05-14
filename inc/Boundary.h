/*
 * Boundary.h
 *
 *  Created on: 27 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef BOUNDARY_H_
#define BOUNDARY_H_

#include <vector>
using std::vector;

/// \class Boundary
/// \brief Mesh boundary nodes
class Boundary {

public:

	/// \enum BoundaryType
	/// \brief Determines the type of restrictions to be imposed to the mesh.
	enum BoundaryType{FIXED,SLIDING};

	/// \struct planeBoundary
	/// \brief Represents the nodes and the type of restrictions
	struct planeBoundary
	{
		BoundaryType type; //!< type of restrictions to be applied to the nodes
		vector<int> nodes; //!< nodes in plane
	};

	Boundary(); //!< default constructor
	virtual ~Boundary(); //!< default destructor

	const planeBoundary* getPlaneX0(); //!< returns plane X0
	const planeBoundary* getPlaneY0(); //!< returns plane Y0
	const planeBoundary* getPlaneZ0(); //!< returns plane Z0
	const planeBoundary* getPlaneXn(); //!< returns plane Xn
	const planeBoundary* getPlaneYn(); //!< returns plane Yn
	const planeBoundary* getPlaneZn(); //!< returns plane Zn

	void setNodesPlaneX0(vector<int>); //!< configures the plane X0 
	void setNodesPlaneY0(vector<int>); //!< configures the plane Y0
	void setNodesPlaneZ0(vector<int>); //!< configures the plane Z0
	void setNodesPlaneXn(vector<int>); //!< configures the plane Xn
	void setNodesPlaneYn(vector<int>); //!< configures the plane Yn
	void setNodesPlaneZn(vector<int>); //!< configures the plane Zn

private:

	Boundary::planeBoundary planeX0; //!< boundary nodes at plane X0
	Boundary::planeBoundary planeY0; //!< boundary nodes at plane Y0 
	Boundary::planeBoundary planeZ0; //!< boundary nodes at plane Z0 
	Boundary::planeBoundary planeXn; //!< boundary nodes at plane Xn 
	Boundary::planeBoundary planeYn; //!< boundary nodes at plane Yn 
	Boundary::planeBoundary planeZn; //!< boundary nodes at plane Zn 
};

#endif /* BOUNDARY_H_ */
