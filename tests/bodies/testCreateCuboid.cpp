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
#include "Elastic.h"

int main(int argc, char **argv)
{
	// create the mesh
	Mesh mesh;
	mesh.setNumCells(10,10,10);
	mesh.setCellDimension(1,1,1);
	mesh.setNumGhosts(2);
	mesh.createGrid();

	// create two materials
	Elastic elastic1(10,1.0,1.0,0.3);
	Elastic elastic2(20,1.0,1.0,0.3);

	// create a body using the configured materials
	Body body;

	// set materials in the body vector materials
	body.setMaterialVector(elastic1);
	body.setMaterialVector(elastic2);

#if 0
	// create a cuboid by a reference
	body.createCuboid(mesh, Vector3d(0,0,0),Vector3d(2,3,5),body.getMaterials()->at(0));
	body.createCuboid(mesh, Vector3d(2,2,6),Vector3d(4,4,7),body.getMaterials()->at(1));
#else
	// create a cuboid by the id of the materials
	body.createCuboid(mesh, Vector3d(0,0,0),Vector3d(2,3,5),10);
	body.createCuboid(mesh, Vector3d(2,2,6),Vector3d(4,4,7),20);
#endif
	// create an output object
	Output output;
	output.writeGrid(mesh,Output::POINTS);
	output.writeBody(body);

	return 0;
}


