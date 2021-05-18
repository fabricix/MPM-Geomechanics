/*
 * explicitUSL.cpp
 *
 *  Created on: 17 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */




/*
 * testMesh.cpp
 *
 *  Created on: 23 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <MPM.h>

int main(int argc, char **argv)
{
	// create a mpm master instance
	MPM mpm;

	// read the input file
	mpm.readInputFile(argc,argv);

	// create the model
	mpm.createModel();

	// solve the problem in time
	mpm.solve();

	// finish the program
	mpm.end();
	
	return 0;
}
