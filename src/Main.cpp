// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include "MPM.h"
#include "Output.h"

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

	// save state
	mpm.saveState();

	// finish the program
	mpm.end();

	return 0;
}
