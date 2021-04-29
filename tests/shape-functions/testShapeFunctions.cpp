/*
 * testShapeFunctions.cpp
 *
 *  Created on: 23 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;

#include "Shape.h"

int main(int argc, char **argv)
{
	// shape instance
	Shape shape;

	// node position
	Vector3d nodePosition(0.0,0.0,0.0);

	// domain
	Vector3d iDomain(-3,-3,-3);
	Vector3d fDomain(3,3,3);

	// increments
	double nIncrements=300;

	// increments in each direction
	double dx = (fDomain.x()-iDomain.x())/nIncrements;
	double dy = (fDomain.y()-iDomain.y())/nIncrements;
	double dz = (fDomain.z()-iDomain.z())/nIncrements;

	// cell dimension
	Vector3d cellDim(1.0,1.0,1.0);

	// particle size
	Vector3d particleSize(0.5,0.5,0.5);

	// node position
	Vector3d gx(0.5,0.5,0.5);

	// open a file
	ofstream outputfile;
	outputfile.open ("shape.csv");
	outputfile<<"x,y,z,sx,sy,sz,gx,gy,gz\n";

	// compute shape and gradients
	for(size_t i=0; i<nIncrements;i++){

		// particle position
		Vector3d pPosition(iDomain.x()+dx*i,iDomain.y()+dy*i,iDomain.z()+dz*i);

		shape.updateGimp(pPosition,gx,cellDim,particleSize);
		outputfile<<pPosition.x()<<","<<pPosition.y()<<","<<pPosition.z()<<","<<shape.Sx<<","<<shape.Sy<<","<<shape.Sz<<","<<shape.Gx<<","<<shape.Gy<<","<<shape.Gz<<"\n";
	}
	outputfile.close();
	return 0;
}