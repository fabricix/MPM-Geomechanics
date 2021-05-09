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
	bool createCuboid(Mesh&, Vector3d, Vector3d, Material*); //<! creates a cuboid

	// get methods
	int getId(); //<! returns the body identification
	
	vector<Particle>& getParticles(); //<! returns particle in the body
	
	// set methods
	void setId(int); //<! configures the id of the body
	
	// static methods
	static int getTotalBodies(); //<! returns o number of bodies
	
private:
	
	int id; //!< body identification
	
	vector<Particle> particles; //!< material points forming the body  

	static int totalBodies; //!< number of bodies
};

#endif /* BODY_H_ */
