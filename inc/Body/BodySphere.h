/*
 * BodySphere.h
 *
 *  Created on: 30 de ago de 2024
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef BODYSPHERE_H_
#define BODYSPHERE_H_

#include <Body/Body.h>

/// \class BodySphere
/// \brief Is a type of Body representing a sphere in 3D
/// 
/// ### Sphere definition
/// 
/// The sphere is geometrically defined by its center and diameter. 
///

class BodySphere: public Body {

public:

	/// \brief Default constructor
	///
	BodySphere();
	
	/// \brief Default destructor
	///
	virtual ~BodySphere();

	inline void setDiameter(double diameter) { this->diameter = diameter; }
	
	inline void setCenter( Vector3d center) { this->center = center; }

	/// \brief Configure the material id
	/// \param[in] material_id Material identification
	inline virtual void setMaterialId(int material_id) { this->materialId=material_id; }
	
	/// \brief Return the material identification
	/// \return Material identification
	inline virtual int getMaterialId() const { return this->materialId; }

    inline void setParticlesPerDirection(Vector3d ppd) { this->particlesPerDirection = ppd; }

    virtual void create(Mesh& mesh, Material* material);

	/// \brief Create a sphere in space
	//virtual void create(Mesh& mesh, Material* material, const Vector3d& center, double diameter, const Vector3i& particlesInDirection);

private:

	Vector3d center; //!< center of the sphere

    Vector3d particlesPerDirection; //!< particles per direction

	double diameter; //!< diameter of the sphere
	
	int materialId; //!< material identification

};

#endif /* BODYSPHERE_H_ */
