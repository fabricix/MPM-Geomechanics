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

class MPM {

public:
	MPM();
	virtual ~MPM();

private:

	Model model; //! model setup
	Mesh mesh; //!< grid mesh
	vector<Particle> particles; //!< material points
};

#endif /* MPM_H_ */
