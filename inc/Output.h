/*
 * Output.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "Mesh.h"
#include "Particle.h"

enum GridType{POINTS,CELLS};

class Output {

public:

	Output();
	virtual ~Output();
	void writeGrid(Mesh&,GridType=GridType::CELLS); // write the grid mesh in to a vtu file
	void writeParticles(vector<Particle>&, double=0.0);

private:

	void defineEdian();
	double checkTolerance(double);
	void createGridFolder();
	void createParticleFolder();
};

#endif /* OUTPUT_H_ */
