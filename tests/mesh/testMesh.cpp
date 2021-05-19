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

#include "Mesh.h"
#include "Output.h"

int main(int argc, char **argv)
{
	// create the mesh
	Mesh mesh;
	mesh.setNumCells(10,10,10);
	mesh.setCellDimension(1.0,1.0,1.0);
	mesh.setNumGhosts(2);
	mesh.createGrid();

	// create particles
	std::vector<Particle*> particles;
	particles.push_back(new Particle(Vector3d(0.4,0.4,0.4)));
	particles.push_back(new Particle(Vector3d(2.5,0.5,0.5)));
	particles.push_back(new Particle(Vector3d(7.9,1.4,8.5)));

	// activate nodes containing p1
	mesh.activateNodes(mesh.getNodesInCell(particles.at(0)->getPosition()));

	// activate contribution nodes of p2
	mesh.activateNodes(mesh.getContributionNodes(particles.at(1)->getPosition()));

	// activate contribution nodes of p3
	mesh.activateNodes(mesh.getContributionNodes(particles.at(2)->getPosition()));

	// write the mesh
	Output::writeGrid(mesh,Output::POINTS);

	// write the particles
	Output::writeParticles(particles);
	
	return 0;
}
