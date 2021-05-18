/*
 * BodyCuboid.h
 *
 *  Created on: 17 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef BODYCUBOID_H_
#define BODYCUBOID_H_

#include <Body.h>

class BodyCuboid: public Body {

public:

	BodyCuboid();
	virtual ~BodyCuboid();

	void setPoints(Vector3d,Vector3d);//!< configures the cuboid points
	
	virtual void setMaterialId(int);//!< configures the material id
	virtual int getMaterialId(); //!< returns the material id

	virtual void create(Mesh& mesh, Material* material); //!< create a cuboid

private:

	Vector3d pointP1;
	Vector3d pointP2;
	int materialId; 
};

#endif /* BODYCUBOID_H_ */
