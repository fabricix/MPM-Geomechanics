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

	BodyCuboid(); //!< default constructor
	virtual ~BodyCuboid(); //!< default destructor

	void setPoints(Vector3d,Vector3d);//!< configures the cuboid points
	
	virtual void setMaterialId(int); //!< configures the material id
	virtual int getMaterialId(); //!< returns the material id

	virtual void create(Mesh& mesh, Material* material); //!< create a cuboid

private:

	Vector3d pointP1; //!< lower left point
	Vector3d pointP2; //!< higher right point
	int materialId; //!< material identification
};

#endif /* BODYCUBOID_H_ */
