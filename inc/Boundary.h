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
	/// \brief Determines the type of fixities to be imposed to the mesh. 
	enum BoundaryType{FIXED,SYMETRICAL};

	Boundary();
	virtual ~Boundary();

public:
	
	BoundaryType type; //!< type of fixities to be applied to the nodes 

	vector<int> planeX0; //!< nodes in plane X0
	vector<int> planeY0; //!< nodes in plane Y0 
	vector<int> planeZ0; //!< nodes in plane Z0 
	vector<int> planeXn; //!< nodes in plane Xn 
	vector<int> planeYn; //!< nodes in plane Yn 
	vector<int> planeZn; //!< nodes in plane Zn 
};

#endif /* BOUNDARY_H_ */
