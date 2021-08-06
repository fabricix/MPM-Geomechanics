/*
 * BodyPolygon.cpp
 *
 *  Created on: 16 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Body/BodyPolygon.h"
#include "Geometry.h"
#include "Model.h"
#include "Particle/ParticleMixture.h"

#include <math.h>

BodyPolygon::BodyPolygon():
materialId(0),extrudeDirection(""),extrudeDisplacement(0),distretizationLength(0) {

}

BodyPolygon::~BodyPolygon() {

}

void BodyPolygon::create(Mesh& mesh, Material* material) {

	// cell dimension
	double cellDimension = this->distretizationLength;

	// particles in each direction
	Vector3i particlesInDirection(2,2,2);

	// particle size
	Vector3d particleSize;
	particleSize.x()=cellDimension/particlesInDirection.x();
	particleSize.y()=cellDimension/particlesInDirection.y();
	particleSize.z()=cellDimension/particlesInDirection.z();

	// calculate the bounding box of the points
	vector<Vector3d> box;
	Geometry::getBoundingBox(this->polygonPoints,box);

	Vector3d minPoint = box.at(0); // left-lower point
	Vector3d maxPoint = box.at(1); // right-upper point

	// reference cell dimension
	double dx = cellDimension;
	double dy = cellDimension;
	double dz = cellDimension;

	// compute number of cells in each direction
	int ncellx(0), ncelly(0), ncellz(0);

	if (extrudeDirection=="y")
	{
		ncellx = (int)std::ceil((maxPoint.x()-minPoint.x())/dx);
		ncelly = (int)std::ceil(this->extrudeDisplacement/dy);
		ncellz = (int)std::ceil((maxPoint.z()-minPoint.z())/dz);
	}
	else if (extrudeDirection=="x")
	{
		ncellx = (int)std::ceil(this->extrudeDisplacement/dx);		
		ncelly = (int)std::ceil((maxPoint.y()-minPoint.y())/dy);
		ncellz = (int)std::ceil((maxPoint.z()-minPoint.z())/dz);
	}
	else if (extrudeDirection=="z")
	{
		ncellx = (int)std::ceil((maxPoint.x()-minPoint.x())/dx);
		ncelly = (int)std::ceil((maxPoint.y()-minPoint.y())/dy);
		ncellz = (int)std::ceil(this->extrudeDisplacement/dz);
	}

	// verify if is two phase calculations
	bool is_two_phase = ModelSetup::getTwoPhaseActive();

	// reference point
	Vector3d p1 = minPoint;

	// create the body inside the polygon
	for (int i = 0; i < ncellx; ++i)
	{    
		for (int j = 0; j < ncelly; ++j)
		{
			for (int k = 0; k < ncellz; ++k)
			{ 
				// 3D reference cell
				//    7----8
				//   /|   /|
				//  5----6 |
				//  | 3--|-4
				//  |/   |/
				//  1----2
				// 
				Vector3d pt1 = p1  + Vector3d(dx*i,dy*j,dz*k);
				Vector3d pt2 = pt1 + Vector3d(dx,0.0,0.0);
				Vector3d pt3 = pt1 + Vector3d(0.0,dy,0.0);
				Vector3d pt4 = pt2 + Vector3d(0.0,dy,0.0);
				
				Vector3d pt5 = pt1 + Vector3d(0.0,0.0,dz);
				Vector3d pt6 = pt2 + Vector3d(0.0,0.0,dz);
				Vector3d pt7 = pt3 + Vector3d(0.0,0.0,dz);
				Vector3d pt8 = pt4 + Vector3d(0.0,0.0,dz);

				// modify the points to positioning particles inside the reference cell

				// particle 1
				pt1.x() += (dx*0.25);
				pt1.y() += (dy*0.25);
				pt1.z() += (dz*0.25);

				// particle 2
				pt2.x() -= (dx*0.25);
				pt2.y() += (dy*0.25);
				pt2.z() += (dz*0.25);

				// particle 3
				pt3.x() += (dx*0.25);
				pt3.y() -= (dy*0.25);
				pt3.z() += (dz*0.25);

				// particle 4
				pt4.x() -= (dx*0.25);
				pt4.y() -= (dy*0.25);
				pt4.z() += (dz*0.25);

				// particle 5
				pt5.x() += (dx*0.25);
				pt5.y() += (dy*0.25);
				pt5.z() -= (dz*0.25);

				// particle 6
				pt6.x() -= (dx*0.25);
				pt6.y() += (dy*0.25);
				pt6.z() -= (dz*0.25);

				// particle 7
				pt7.x() += (dx*0.25);
				pt7.y() -= (dy*0.25);
				pt7.z() -= (dz*0.25);

				// particle 8
				pt8.x() -= (dx*0.25);
				pt8.y() -= (dy*0.25);
				pt8.z() -= (dz*0.25);
				
				// check if the points are inside the polygon
				if(Geometry::getInsidePolygon(this->polygonPoints,pt1,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt1,material,particleSize) : new Particle(pt1,material,particleSize));
				if(Geometry::getInsidePolygon(this->polygonPoints,pt2,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt2,material,particleSize) : new Particle(pt2,material,particleSize));
				if(Geometry::getInsidePolygon(this->polygonPoints,pt3,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt3,material,particleSize) : new Particle(pt3,material,particleSize));
				if(Geometry::getInsidePolygon(this->polygonPoints,pt4,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt4,material,particleSize) : new Particle(pt4,material,particleSize));
				if(Geometry::getInsidePolygon(this->polygonPoints,pt5,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt5,material,particleSize) : new Particle(pt5,material,particleSize));
				if(Geometry::getInsidePolygon(this->polygonPoints,pt6,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt6,material,particleSize) : new Particle(pt6,material,particleSize));
				if(Geometry::getInsidePolygon(this->polygonPoints,pt7,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt7,material,particleSize) : new Particle(pt7,material,particleSize));
				if(Geometry::getInsidePolygon(this->polygonPoints,pt8,this->extrudeDirection)) Body::particles.push_back(is_two_phase ? new ParticleMixture(pt8,material,particleSize) : new Particle(pt8,material,particleSize));
			}
		}
	}
	// release memory
	polygonPoints.clear();
}
