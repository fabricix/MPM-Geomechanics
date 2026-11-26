/*
 * testShapeFunctions.cpp
 *
 *  Created on: 23 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <string>
using std::string;

#include <fstream>
using std::ofstream;

#include "ShapeGimp.h"
#include "ShapeLinear.h"

void computeShapeAndGradients(
		Shape* shapePntr,
		string filenameStr,
		double nIncrements,
		Vector3d iDomain,
		Vector3d delta,
		Vector3d gx,
		Vector3d cellDim,
		Vector3d particleSize)
{
	// open a file
	ofstream outputfile;
	outputfile.open (filenameStr);
	outputfile<<"x,y,z,weight,gx,gy,gz\n";

	// compute shape and gradients
	for(size_t i=0; i<nIncrements;i++){

		// particle position
		Vector3d pPosition(iDomain.x()+delta.x()*i,iDomain.y()+delta.y()*i,iDomain.z()+delta.z()*i);

		// update shape function and derivates
		shapePntr->update(pPosition,gx,cellDim,particleSize);

		// Complete nodal weight and spatial gradient, as used by Particle.
		const double weight = shapePntr->getWeight();
		const Vector3d gradient = shapePntr->getGradient();

		// write the results
		outputfile
		<<pPosition.x()<<","<<pPosition.y()<<","<<pPosition.z()<<","
		<<weight<<","
		<<gradient.x()<<","<<gradient.y()<<","<<gradient.z()<<"\n";
	}
	outputfile.close();
}

int main()
{
	// shape instance
	ShapeGimp shapeGimp;
	ShapeLinear shapeLinear;

	// domain
	Vector3d iDomain(-3,-3,-3);
	Vector3d fDomain(3,3,3);

	// increments
	double nIncrements=300;

	// increments in each direction
	Vector3d delta = (fDomain-iDomain)/nIncrements;

	// cell dimension
	Vector3d cellDim(1.0,1.0,1.0);

	// particle size
	Vector3d particleSize(0.5,0.5,0.5);

	// node position
	Vector3d gx(0.5,0.5,0.5);

	// write linear shape function
	computeShapeAndGradients(&shapeLinear,"shapeLinear.csv",nIncrements,iDomain,delta,gx,cellDim,particleSize);

	// write gimp shape functions
	computeShapeAndGradients(&shapeGimp,"shapeGimp.csv",nIncrements,iDomain,delta,gx,cellDim,particleSize);

	
	return 0;
}
