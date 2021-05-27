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

	/// \brief Default constructor
	///
	Boundary();

	/// \brief Default destructor
	///
	virtual ~Boundary();

	/// \brief Return the nodes lower than the plane X0, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	const planeBoundary* getPlaneX0();

	/// \brief Return the nodes lower than the plane Y0, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	const planeBoundary* getPlaneY0();

	/// \brief Return the nodes lower than the plane Z0, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	const planeBoundary* getPlaneZ0();
	
	/// \brief Return the nodes lower than the plane Xn, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	const planeBoundary* getPlaneXn();
	
	/// \brief Return the nodes lower than the plane Yn, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	const planeBoundary* getPlaneYn();
	
	/// \brief Return the nodes lower than the plane Zn, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary	
	const planeBoundary* getPlaneZn();


	/// \brief Configure the nodes lower than the plane X0, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	void setNodesPlaneX0(vector<int>);
	
	/// \brief Configure the nodes lower than the plane Y0, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	void setNodesPlaneY0(vector<int>);
	
	/// \brief Configure the nodes lower than the plane Z0, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	void setNodesPlaneZ0(vector<int>);
	
	/// \brief Configure the nodes lower than the plane Xn, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	void setNodesPlaneXn(vector<int>);
	
	/// \brief Configure the nodes lower than the plane Yn, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	void setNodesPlaneYn(vector<int>);
	
	/// \brief Configure the nodes lower than the plane Zn, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	void setNodesPlaneZn(vector<int>);

private:

	Boundary::planeBoundary planeX0; //!< boundary nodes at plane X0

	Boundary::planeBoundary planeY0; //!< boundary nodes at plane Y0 

	Boundary::planeBoundary planeZ0; //!< boundary nodes at plane Z0 

	Boundary::planeBoundary planeXn; //!< boundary nodes at plane Xn 

	Boundary::planeBoundary planeYn; //!< boundary nodes at plane Yn 
	
	Boundary::planeBoundary planeZn; //!< boundary nodes at plane Zn 
};

#endif /* BOUNDARY_H_ */
