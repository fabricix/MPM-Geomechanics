/*
 * BodyShere.cpp
 *
 *  Created on: 30 de ago 2024
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Body/BodySphere.h"
#include "Particle/ParticleMixture.h"
#include "Model.h"

BodySphere::BodySphere(): diameter(1.0), center(0,0,0), particlesPerDirection(2,2,2) {
	
    materialId=-1;
}

BodySphere::~BodySphere() { }

void BodySphere::create(Mesh& mesh, Material* material) {

    // cell dimension
    Vector3d cellDimension = mesh.getCellDimension();

	// particles in each direction
    Vector3d particlesInDirection = this->particlesPerDirection;

    // particle size
    Vector3d particleSize;
    particleSize.x() = cellDimension.x() / particlesInDirection.x();
    particleSize.y() = cellDimension.y() / particlesInDirection.y();
    particleSize.z() = cellDimension.z() / particlesInDirection.z();

    // radius of the sphere
    double radius = diameter / 2.0;

    // verify if it's two phase calculations
    bool is_two_phase = ModelSetup::getTwoPhaseActive();

    // calculate the bounding box of the sphere
    Vector3d minPoint = center - Vector3d(radius, radius, radius);
    Vector3d maxPoint = center + Vector3d(radius, radius, radius);

    // calculate the number of cells in each direction
    Vector3i nCellsBoundingBox;
    nCellsBoundingBox.x() = std::ceil((maxPoint.x() - minPoint.x()) / cellDimension.x());
    nCellsBoundingBox.y() = std::ceil((maxPoint.y() - minPoint.y()) / cellDimension.y());
    nCellsBoundingBox.z() = std::ceil((maxPoint.z() - minPoint.z()) / cellDimension.z());

    // create the particles within the sphere
    for (int i = 0; i < nCellsBoundingBox.x(); ++i) {
        for (int j = 0; j < nCellsBoundingBox.y(); ++j) {
            for (int k = 0; k < nCellsBoundingBox.z(); ++k) {

                // calculate the base position of the current cell
                Vector3d cellBase = minPoint + Vector3d(i * cellDimension.x(), j * cellDimension.y(), k * cellDimension.z());

                // generate particles within the cell
                for (int p = 0; p < particlesInDirection.x(); ++p) {
                    for (int q = 0; q < particlesInDirection.y(); ++q) {
                        for (int r = 0; r < particlesInDirection.z(); ++r) {

                            // calculate the position of the particle
                            Vector3d particlePos = cellBase + Vector3d(
                                (p + 0.5) * particleSize.x(),
                                (q + 0.5) * particleSize.y(),
                                (r + 0.5) * particleSize.z()
                            );

                            // check if the particle is within the sphere
                            if ((particlePos - center).norm() <= radius) {
                                Body::particles.push_back(is_two_phase ? new ParticleMixture(particlePos, material, particleSize) : new Particle(particlePos, material, particleSize));
                            }
                        }
                    }
                }
            }
        }
    }
}
