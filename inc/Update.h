/*
 * Update.h
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include "Mesh.h"
#include "Particle.h"

/// \class Update
/// \brief Represents operations to update values in nodes and particles.
class Update {

public:

	Update(); //!< default constructor
	virtual ~Update(); //!< default destructor

	void nodalVelocity(Mesh&); //!< updates the nodal velocity
	void resetNodalValues(Mesh&); //!< deletes all stored values in nodes 
	void particleDensity(vector<Particle>&); //!< updates the particles' density
	void particleStress(vector<Particle>&); //!< update the particles' stress
	void particleVelocity(Mesh&, vector<Particle>&,double); //!< update the particle velocity
	void particlePosition(Mesh&, vector<Particle>&,double); //!< update the particle position
};

#endif /* UPDATE_H_ */
