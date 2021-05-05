/*
 * Body.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef BODY_H_
#define BODY_H_

#include<vector>
using std::vector;

#include "Particle.h"
#include "Material.h"

/// \class Body
/// \brief Represents a body
class Body {

public:
	
	Body();
	virtual ~Body();
	
	// create methods
	bool createCuboid(Mesh&, Vector3d, Vector3d, Material&); //<! creates a cuboid
	bool createCuboid(Mesh& mesh, Vector3d pointP1, Vector3d pointP2, int materialId); //<! creates a cuboid

	// get methods
	int getId(); //<! returns the body identification
	
	vector<Particle>& getParticles(); //<! returns particle in the body

	vector<Material>* getMaterials(); //<! returns the materials vector
	
	// set methods
	void setId(int); //<! configures the id of the body

	static void setMaterialVector(Material); //<! configures a material in the body
	
	// static methods
	static int getTotalBodies(); //<! returns o number of bodies
	
private:
	
	int id; //!< body identification
	
	vector<Particle> particles; //!< material points forming the body  
	
	static vector<Material> materials; //!< materials used in all created bodies

	static int totalBodies; //!< number of bodies
};

#endif /* BODY_H_ */
