/*
 * Output.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "../inc/Mesh.h"

class Output {

public:

	Output();
	virtual ~Output();
	void writeGrid(Mesh&); // write the grid mesh in to a vtu file

private:

	void defineEdian();
	double checkTolerance(double);
	void createGridFolder();
};

#endif /* OUTPUT_H_ */
