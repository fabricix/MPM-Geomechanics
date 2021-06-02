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

	enum Plane {X0,Y0,Z0,Xn,Yn,Zn};

	Plane boundaryPlane=Plane::Z0;

	switch (boundaryPlane)
	{
		case Plane::X0:
			// active nodes in plane X0
			mesh.activateNodes(mesh.getBoundary()->getPlaneX0()->nodes);
			break;

		case Plane::Y0:
			// active nodes in plane Y0
			mesh.activateNodes(mesh.getBoundary()->getPlaneY0()->nodes);
			break;

		case Plane::Z0:
			// active nodes in plane Z0
			mesh.activateNodes(mesh.getBoundary()->getPlaneZ0()->nodes);
			break;

		case Plane::Xn:
			// active nodes in plane Xn
			mesh.activateNodes(mesh.getBoundary()->getPlaneXn()->nodes);
			break;

		case Plane::Yn:
			// active nodes in plane Yn
			mesh.activateNodes(mesh.getBoundary()->getPlaneYn()->nodes);
			break;

		case Plane::Zn:
			// active nodes in plane Zn
			mesh.activateNodes(mesh.getBoundary()->getPlaneZn()->nodes);
			break;
	}

	// write the mesh
	Output::writeGrid(mesh,Output::POINTS);

	
	return 0;
}
