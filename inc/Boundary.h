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
	inline const planeBoundary* getPlaneX0() { return &(this->planeX0); }

	/// \brief Return the nodes lower than the plane Y0, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	inline const planeBoundary* getPlaneY0() { return &(this->planeY0); }

	/// \brief Return the nodes lower than the plane Z0, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	inline const planeBoundary* getPlaneZ0() { return &(this->planeZ0); }
	
	/// \brief Return the nodes lower than the plane Xn, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	inline const planeBoundary* getPlaneXn() { return &(this->planeXn); }
	
	/// \brief Return the nodes lower than the plane Yn, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary
	inline const planeBoundary* getPlaneYn()  { return &(this->planeYn); }
	
	/// \brief Return the nodes lower than the plane Zn, including ghosts
	/// \param[out] *plane_boundary A pointer to planeBoundary	
	inline const planeBoundary* getPlaneZn()  { return &(this->planeZn); }

	/// \brief Configure the nodes lower than the plane X0, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	inline void setNodesPlaneX0(vector<int> nodes_id) { this->planeX0.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Y0, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	inline void setNodesPlaneY0(vector<int> nodes_id) { this->planeY0.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Z0, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	inline void setNodesPlaneZ0(vector<int> nodes_id) { this->planeZ0.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Xn, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	inline void setNodesPlaneXn(vector<int> nodes_id) { this->planeXn.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Yn, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	inline void setNodesPlaneYn(vector<int> nodes_id) { this->planeYn.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Zn, including ghosts
	/// \param[int] nodes_id A vector with nodes identifications
	inline void setNodesPlaneZn(vector<int> nodes_id) { this->planeZn.nodes=nodes_id; }

private:

	Boundary::planeBoundary planeX0; //!< boundary nodes at plane X0

	Boundary::planeBoundary planeY0; //!< boundary nodes at plane Y0 

	Boundary::planeBoundary planeZ0; //!< boundary nodes at plane Z0 

	Boundary::planeBoundary planeXn; //!< boundary nodes at plane Xn 

	Boundary::planeBoundary planeYn; //!< boundary nodes at plane Yn 
	
	Boundary::planeBoundary planeZn; //!< boundary nodes at plane Zn 
};

inline Boundary::Boundary(){

	planeX0.type=BoundaryType::SLIDING;
	planeY0.type=BoundaryType::SLIDING;
	planeZ0.type=BoundaryType::SLIDING;
	planeXn.type=BoundaryType::SLIDING;
	planeYn.type=BoundaryType::SLIDING;
	planeZn.type=BoundaryType::SLIDING;
}

inline Boundary::~Boundary() {
	
}

#endif /* BOUNDARY_H_ */
