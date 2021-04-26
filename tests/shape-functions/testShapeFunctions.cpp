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
	double dx = (fDomain(0)-iDomain(0))/nIncrements;
	double dy = (fDomain(1)-iDomain(1))/nIncrements;
	double dz = (fDomain(2)-iDomain(2))/nIncrements;

	// cell dimension
	Vector3d cellDim(1.0,1.0,1.0);

	// particle size
	Vector3d particleSize(0.5,0.5,0.5);

	// node position
	Vector3d gx(0.5,0.5,0.5);

	// open a file
	ofstream outputfile;
	outputfile.open ("../tests/shape-functions/shape.csv");
	outputfile<<"x,y,z,sx,sy,sz,gx,gy,gz\n";

	// compute shape and gradients
	for(size_t i=0; i<nIncrements;i++){

		// particle position
		Vector3d px(iDomain(0)+dx*i,iDomain(1)+dy*i,iDomain(2)+dz*i);

		shape.updateGimp(px,gx,cellDim,particleSize);
		outputfile<<px(0)<<","<<px(1)<<","<<px(2)<<","<<shape.Sx<<","<<shape.Sy<<","<<shape.Sz<<","<<shape.Gx<<","<<shape.Gy<<","<<shape.Gz<<"\n";
	}
	outputfile.close();
	return 0;
}
