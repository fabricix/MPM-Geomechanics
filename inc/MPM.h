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

/// \class MPM
/// \brief Components and algorithms of the MPM
/// This class contain the basic objects of a MPM model,
/// and the operations between them.
class MPM {

public:

	MPM(); //!< default constructor
	virtual ~MPM(); //!< default destructor
	void updateParticleContribution(); //!< updates the weights of all nodes the particles contribute
	void printMesh(); //!< writes the mesh in the mesh file for its visualization
	void printParticles(); //!< writes the particle file for its visualization

public:
	
	Mesh mesh; //!< grid mesh
	vector<Particle> particles; //!< material points
	Output output; //!< output operations
};

#endif /* MPM_H_ */
