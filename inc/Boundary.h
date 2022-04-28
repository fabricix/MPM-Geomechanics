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
///
/// ### Definition of the planes for setting boundary conditions
///
/// ```
///                     Boundaries of the Mesh
///                             
///                          +----------+
///                          |\          \ 
///                          | \ Plane Zn \ 
///    z                     |  \          \ 
///    |       Plane Y0 ------>  +----------+  <------ Plane Yn
///    |                     |   |          |
///    +---- y   		     +   |          |       
///     \                     \  | Plane Xn |
///      \                     \ |          |
///       x                     \|          |
///                              +----------+ 
///
///
/// Plane X0 : Plane which normal points to the negative direction of axis X
/// Plane Xn : Plane which normal points to the positive direction of axis X
///
/// ```
/// **Note**: If any boundary condition is defined in the input file, sliding boundary condition
/// is chosen, by default, for all planes.
///
class Boundary {

public:

	/// \enum BoundaryType
	/// \brief Determines the type of restrictions to be imposed to the mesh.
	enum BoundaryType{ FREE, FIXED, SLIDING };

	/// \enum BoundaryPlane
	/// \brief Planes at the mesh boundary
	enum BoundaryPlane{ X0, Y0, Z0, Xn, Yn, Zn };

	/// \struct planeBoundary
	/// \brief Represents the nodes and the type of restrictions
	struct planeBoundary
	{
		BoundaryType restriction; //!< type of restrictions to be applied to the solid phase
		BoundaryType restrictionFluid; //!< type of restrictions to be applied to the fluid phase
		vector<int> nodes; //!< nodes in plane
	};

	/// \brief Default constructor
	///
	Boundary();

	/// \brief Default destructor
	///
	virtual ~Boundary();

	/// \brief Return the nodes lower than the plane X0, including ghosts
	/// \return A pointer to planeBoundary in X0
	inline const planeBoundary* getPlaneX0() const { return &(this->planeX0); }

	/// \brief Return the nodes lower than the plane Y0, including ghosts
	/// \return A pointer to planeBoundary in Y0
	inline const planeBoundary* getPlaneY0() const { return &(this->planeY0); }

	/// \brief Return the nodes lower than the plane Z0, including ghosts
	/// \return A pointer to planeBoundary in Z0
	inline const planeBoundary* getPlaneZ0() const { return &(this->planeZ0); }
	
	/// \brief Return the nodes lower than the plane Xn, including ghosts
	/// \return A pointer to planeBoundary Xn
	inline const planeBoundary* getPlaneXn() const { return &(this->planeXn); }
	
	/// \brief Return the nodes lower than the plane Yn, including ghosts
	/// \return A pointer to planeBoundary Yn
	inline const planeBoundary* getPlaneYn() const { return &(this->planeYn); }
	
	/// \brief Return the nodes lower than the plane Zn, including ghosts
	/// \return A pointer to planeBoundary Zn
	inline const planeBoundary* getPlaneZn() const { return &(this->planeZn); }

	/// \brief Configure the nodes lower than the plane X0, including ghosts
	/// \param[in] nodes_id A vector with nodes identifications
	inline void setNodesPlaneX0(const vector<int>& nodes_id) { this->planeX0.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Y0, including ghosts
	/// \param[in] nodes_id A vector with nodes identifications
	inline void setNodesPlaneY0(const vector<int>& nodes_id) { this->planeY0.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Z0, including ghosts
	/// \param[in] nodes_id A vector with nodes identifications
	inline void setNodesPlaneZ0(const vector<int>& nodes_id) { this->planeZ0.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Xn, including ghosts
	/// \param[in] nodes_id A vector with nodes identifications
	inline void setNodesPlaneXn(const vector<int>& nodes_id) { this->planeXn.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Yn, including ghosts
	/// \param[in] nodes_id A vector with nodes identifications
	inline void setNodesPlaneYn(const vector<int>& nodes_id) { this->planeYn.nodes=nodes_id; }
	
	/// \brief Configure the nodes lower than the plane Zn, including ghosts
	/// \param[in] nodes_id A vector with nodes identifications
	inline void setNodesPlaneZn(const vector<int>& nodes_id) { this->planeZn.nodes=nodes_id; }

	/// \brief Configure the restrictions of the boundary nodes
	/// \param[in] restriction Type of restriction Boundary::BoundaryType
	inline void setRestrictions(Boundary::BoundaryType restriction);

	/// \brief Configure the restrictions of the boundary nodes
	/// \param[in] restriction Type of restriction Boundary::BoundaryType
	inline void setRestrictionsFluid(Boundary::BoundaryType restriction);

	/// \brief Configure the restrictions of the boundary nodes by plane
	/// \param[in] restriction Type of restriction Boundary::BoundaryType
	/// \param[in] plane Plane to be applied the restriction
	inline void setRestrictions(Boundary::BoundaryPlane plane, Boundary::BoundaryType restriction);

	/// \brief Configure the restrictions of the boundary nodes by plane
	/// \param[in] restriction Type of restriction Boundary::BoundaryType
	/// \param[in] plane Plane to be applied the restriction
	inline void setRestrictionsFluid(Boundary::BoundaryPlane plane, Boundary::BoundaryType restriction);

private:

	Boundary::planeBoundary planeX0; //!< boundary nodes at plane X0

	Boundary::planeBoundary planeY0; //!< boundary nodes at plane Y0 

	Boundary::planeBoundary planeZ0; //!< boundary nodes at plane Z0 

	Boundary::planeBoundary planeXn; //!< boundary nodes at plane Xn 

	Boundary::planeBoundary planeYn; //!< boundary nodes at plane Yn 
	
	Boundary::planeBoundary planeZn; //!< boundary nodes at plane Zn 
};

inline Boundary::Boundary(){

	setRestrictions(Boundary::BoundaryType::SLIDING);
	setRestrictionsFluid(Boundary::BoundaryType::SLIDING);
}

inline Boundary::~Boundary() { }

inline void Boundary::setRestrictions(Boundary::BoundaryType restriction) {
	
	planeX0.restriction=restriction;
	planeY0.restriction=restriction;
	planeZ0.restriction=restriction;
	planeXn.restriction=restriction;
	planeYn.restriction=restriction;
	planeZn.restriction=restriction;
}

inline void Boundary::setRestrictionsFluid(Boundary::BoundaryType restriction) {
	
	planeX0.restrictionFluid=restriction;
	planeY0.restrictionFluid=restriction;
	planeZ0.restrictionFluid=restriction;
	planeXn.restrictionFluid=restriction;
	planeYn.restrictionFluid=restriction;
	planeZn.restrictionFluid=restriction;
}

inline void Boundary::setRestrictions(Boundary::BoundaryPlane plane, Boundary::BoundaryType restriction) {
	
	switch (plane)
	{
		case BoundaryPlane::X0:
			planeX0.restriction=restriction;
			break;
		
		case BoundaryPlane::Y0:
			planeY0.restriction=restriction;
			break;

		case BoundaryPlane::Z0:
			planeZ0.restriction=restriction;
			break;

		case BoundaryPlane::Xn:
			planeXn.restriction=restriction;
			break;
		
		case BoundaryPlane::Yn:
			planeYn.restriction=restriction;
			break;

		case BoundaryPlane::Zn:
			planeZn.restriction=restriction;
			break;
	}
}

inline void Boundary::setRestrictionsFluid(Boundary::BoundaryPlane plane, Boundary::BoundaryType restriction) {
	
	switch (plane)
	{
		case BoundaryPlane::X0:
			planeX0.restrictionFluid=restriction;
			break;
		
		case BoundaryPlane::Y0:
			planeY0.restrictionFluid=restriction;
			break;

		case BoundaryPlane::Z0:
			planeZ0.restrictionFluid=restriction;
			break;

		case BoundaryPlane::Xn:
			planeXn.restrictionFluid=restriction;
			break;
		
		case BoundaryPlane::Yn:
			planeYn.restrictionFluid=restriction;
			break;

		case BoundaryPlane::Zn:
			planeZn.restrictionFluid=restriction;
			break;
	}
}
#endif /* BOUNDARY_H_ */
