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

	Body(); //<! default constructor
	virtual ~Body(); //<! default destructor
	
	virtual void create(Mesh& mesh, Material* material)=0;
	virtual void setMaterialId(int)=0;//!< configures the material id
	virtual int getMaterialId()=0; //!< returns the material id

	int getId(); //<! returns the body identification
	vector<Particle>& getParticles(); //<! returns particle in the body
	static int getTotalBodies(); //<! returns o number of bodies
	
	void setId(int); //<! configures the id of the body
	void setParticles(vector<Particle>); //<! configures the particles
	void insertParticles(vector<Particle>); //<! configures the particles
	
private:
	
	int id; //!< body identification
	vector<Particle> particles; //!< material points forming the body  
	static int totalBodies; //!< number of bodies
};

#endif /* BODY_H_ */