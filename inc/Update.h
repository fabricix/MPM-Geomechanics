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

class Update {

public:

	Update();
	virtual ~Update();

	void nodalVelocity(Mesh&);
	void resetNodalValues(Mesh&);
	void particleDensity(vector<Particle>&);
	void particleStress(vector<Particle>&);
	void particleVelocity(vector<Particle>&,double);
	void particlePosition(vector<Particle>&,double);

};

#endif /* UPDATE_H_ */
