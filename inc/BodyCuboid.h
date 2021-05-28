/*
 * BodyCuboid.h
 *
 *  Created on: 17 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef BODYCUBOID_H_
#define BODYCUBOID_H_

#include <Body.h>

/// \class BodyCuboid
/// \brief Represents cuboid in 3D
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
	inline void setPoints(Vector3d point_1,Vector3d point_2) { this->pointP1=point_1; this->pointP2=point_2; }
	
	/// \brief Configure the material id
	/// \param[in] material_id Material identification
	inline virtual void setMaterialId(int material_id) { this->materialId=material_id; }
	
	/// \brief Return the material identification
	/// \param[out] material_id Material identification
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

inline  BodyCuboid::~BodyCuboid() {
	
}

#endif /* BODYCUBOID_H_ */
