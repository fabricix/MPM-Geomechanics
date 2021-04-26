/*
 * MPM.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MPM_H_
#define MPM_H_

#include "Mesh.h"
#include "Particle.h"
#include "Model.h"
#include "Output.h"

class MPM {

public:
	MPM();
	virtual ~MPM();
	void updateParticleContribution();
	void printMesh();

public:
	
	Mesh mesh; //!< grid mesh
	vector<Particle> particles; //!< material points
	Output output; //!< output operations

};

#endif /* MPM_H_ */
