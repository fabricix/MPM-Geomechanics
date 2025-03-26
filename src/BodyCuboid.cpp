// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Body/BodyCuboid.h"
#include "Particle/ParticleMixture.h"
#include "Model.h"

void BodyCuboid::create(Mesh& mesh, Material* material) {

	// cell dimension
	Vector3d cellDimension = mesh.getCellDimension();

	// particles in each direction
	Vector3i particlesInDirection(2,2,2);

	// particle size
	Vector3d particleSize;
	particleSize.x()=cellDimension.x()/particlesInDirection.x();
	particleSize.y()=cellDimension.y()/particlesInDirection.y();
	particleSize.z()=cellDimension.z()/particlesInDirection.z();
	 
	// number of cells in bounding box
	Vector3d nCellsBoundingBox;
	nCellsBoundingBox.x()=(pointP2.x()-pointP1.x())/cellDimension.x();
	nCellsBoundingBox.y()=(pointP2.y()-pointP1.y())/cellDimension.y();
	nCellsBoundingBox.z()=(pointP2.z()-pointP1.z())/cellDimension.z();
	
	// use a dx,dy,dz notation
	double dx = cellDimension.x();
	double dy = cellDimension.y();
	double dz = cellDimension.z();

	// verify if is two phase calculations
	bool is_two_phase = ModelSetup::getTwoPhaseActive();

	// create the particles and put it in the space
	for (int i = 0; i < nCellsBoundingBox.x(); ++i){

		for (int j = 0; j < nCellsBoundingBox.y(); ++j){

			for (int k = 0; k < nCellsBoundingBox.z(); ++k){ 

				// cell points
				Vector3d pt1 = pointP1+Vector3d(dx*i,dy*j,dz*k);
				Vector3d pt2 = pt1 + Vector3d(dx,0.0,0.0);
				Vector3d pt3 = pt1 + Vector3d(0.0,dy,0.0);
				Vector3d pt4 = pt2 + Vector3d(0.0,dy,0.0);
				Vector3d pt5 = pt1 + Vector3d(0.0,0.0,dz);
				Vector3d pt6 = pt2 + Vector3d(0.0,0.0,dz);
				Vector3d pt7 = pt3 + Vector3d(0.0,0.0,dz);
				Vector3d pt8 = pt4 + Vector3d(0.0,0.0,dz);

				// particles position inside the cell

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

				// push all particles in the body's particle vector
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt1,material,particleSize) : new Particle(pt1,material,particleSize));
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt2,material,particleSize) : new Particle(pt2,material,particleSize));
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt3,material,particleSize) : new Particle(pt3,material,particleSize));
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt4,material,particleSize) : new Particle(pt4,material,particleSize));
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt5,material,particleSize) : new Particle(pt5,material,particleSize));
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt6,material,particleSize) : new Particle(pt6,material,particleSize));
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt7,material,particleSize) : new Particle(pt7,material,particleSize));
				Body::particles.push_back(is_two_phase ? new ParticleMixture(pt8,material,particleSize) : new Particle(pt8,material,particleSize));
			}
		}   
	}
}
