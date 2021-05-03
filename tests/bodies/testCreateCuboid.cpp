/*
 * testParticleContribution.cpp
 *
 *  Created on: 24 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;

#include <string>
using std::to_string;

#include "MPM.h"
#include "Particle.h"
#include "Body.h"

int main(int argc, char **argv)
{
	// create the mesh
	Mesh mesh;
	mesh.setNumCells(10,10,10);
	mesh.setCellDimension(1,1,1);
	mesh.setNumGhosts(2);
	mesh.createGrid();

	// create a body
	Body body;
	body.createCuboid(mesh, Vector3d(0,0,0),Vector3d(2,3,5));

	// create an output object
	Output output;
	output.writeGrid(mesh,Output::POINTS);
	output.writeBody(body);

	return 0;
}


