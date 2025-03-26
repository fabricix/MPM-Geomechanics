// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef BODYCUBOID_H_
#define BODYCUBOID_H_

#include <Body/Body.h>

/// \class BodyCuboid
/// \brief Is a type of Body representing a cuboid in 3D
/// 
/// ### Cuboid definition
/// 
/// The cuboid in geometrically defined by the coordinates of the lower left point (Point 1) 
/// and the higher right point (Point 2). See Figure bellow.
///
/// ```
///	                            Cuboid
///                           ----------
///                          |\         |\ 
///                          | \        | \ 
///    z                     |  \       |  \ 
///    |                     |    ----------+ <-- Point 2
///    |                     |   |      |   |
///    +---- y   Point 1 --> + --|------    |
///     \                     \  |       \  |
///      \                     \ |        \ |
///       x                     \|         \|
///                               ---------- 
/// ```
class BodyCuboid: public Body {

public:

	/// \brief Default constructor
	///
	BodyCuboid();
	
	/// \brief Default constructor
	///
	virtual ~BodyCuboid();

	/// \brief Configure the geometrical points of the cuboid
	/// \param[in] point_1 Lower left geometrical point of the cuboid
	/// \param[in] point_2 Higher right geometrical point of the cuboid
	inline void setPoints(const Vector3d& point_1, const Vector3d& point_2) { this->pointP1=point_1; this->pointP2=point_2; }
	
	/// \brief Configure the material id
	/// \param[in] material_id Material identification
	inline virtual void setMaterialId(int material_id) { this->materialId=material_id; }
	
	/// \brief Return the material identification
	/// \return Material identification
	inline virtual int getMaterialId() const { return this->materialId; }

	/// \brief Create a cuboid in space
	/// \param[in] &mesh Mesh reference
	/// \param[in] *material Material pointer 
	virtual void create(Mesh& mesh, Material* material);

private:

	Vector3d pointP1; //!< lower left point

	Vector3d pointP2; //!< higher right point
	
	int materialId; //!< material identification
};

inline BodyCuboid::BodyCuboid():Body() {
	
	materialId=-1;
}

inline  BodyCuboid::~BodyCuboid() { }

#endif /* BODYCUBOID_H_ */
