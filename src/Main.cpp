/*
 * Main.cpp
 *
 *  Created on: Sun Jun  6 10:03:58 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <MPM.h>
#include <Output.h>

int main(int argc, char **argv)
{
	// print splash
	Output::welcomeScreen();

	// create a mpm instance
	MPM mpm;

	// read the input file
	mpm.readInputFile(argc,argv);

	// create the model
	mpm.createModel();

	// solve the problem in time
	mpm.solve();

	// finish the program
	mpm.end();
	
	// print farewell screen
	Output::farewellScreen();

	return 0;
}
