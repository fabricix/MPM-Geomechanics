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
	Mesh mesh;

	mesh.setNumCells(10,10,10);
	mesh.setCellDimension(1.0,1.0,1.0);
	mesh.setNumGhosts(2);
	mesh.createGrid();

	// activate p1
	mesh.activateNodes(mesh.getNodesInCell(Vector3d(0.4,0.4,0.4)));

	// contribution p2
	mesh.activateNodes(mesh.getContributionNodes(Vector3d(2.5,0.5,0.5)));

	// contribution p3
	mesh.activateNodes(mesh.getContributionNodes(Vector3d(7.9,1.4,8.5)));

	Output output;
	output.writeGrid(mesh,GridType::POINTS);

	return 0;
}
