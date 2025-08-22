// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "TerrainContact.h"
#include "Mesh/Mesh.h"
#include "limits"
#include "Contribution.h"
#include "Particle/Particle.h"
#include "Seismic.h"

#include <utility>  // std::pair

/**
 * @brief Compute the minimum distance from a point to a triangle in 3D.
 *
 * This function determines the shortest distance between a point `p` and a triangle
 * defined by the vertices `a`, `b`, and `c`. It handles all spatial regions around the triangle,
 * including:
 *  - Vertices: returns distance to the closest vertex if the projection falls outside.
 *  - Edges: computes projection onto triangle edges and returns minimum distance if within bounds.
 *  - Inside: if the orthogonal projection of the point falls inside the triangle, 
 *    returns perpendicular distance to the triangle's plane.
 *
 * @param p The point from which the distance is calculated.
 * @param a First vertex of the triangle.
 * @param b Second vertex of the triangle.
 * @param c Third vertex of the triangle.
 * @return The shortest (unsigned) distance from point `p` to the triangle `abc`.
 */
double pointTriangleDistance(const Vector3d& p, const Vector3d& a, const Vector3d& b, const Vector3d& c) {
    // edges
    Vector3d ab = b - a;
    Vector3d ac = c - a;
    Vector3d ap = p - a;

    double d1 = ab.dot(ap);
    double d2 = ac.dot(ap);
    if (d1 <= 0.0 && d2 <= 0.0) return (p - a).norm(); // closest to a

    Vector3d bp = p - b;
    double d3 = ab.dot(bp);
    double d4 = ac.dot(bp);
    if (d3 >= 0.0 && d4 <= d3) return (p - b).norm(); // closest to b

    double vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0) {
        double v = d1 / (d1 - d3);
        return (p - (a + v * ab)).norm(); // closest to edge AB
    }

    Vector3d cp = p - c;
    double d5 = ab.dot(cp);
    double d6 = ac.dot(cp);
    if (d6 >= 0.0 && d5 <= d6) return (p - c).norm(); // closest to c

    double vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0) {
        double w = d2 / (d2 - d6);
        return (p - (a + w * ac)).norm(); // closest to edge AC
    }

    double va = d3 * d6 - d5 * d4;
    if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0) {
        double w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return (p - (b + w * (c - b))).norm(); // closest to edge BC
    }

    // inside triangle
    Vector3d n = (b - a).cross(c - a).normalized();
    return std::abs((p - a).dot(n));
}

void TerrainContact::computeDistanceLevelSetFunction(Mesh* mesh)
{
    std::vector<Node*>* gNodes = mesh->getNodes();
    const std::vector<Triangle>& triangles = stlMesh->getTriangles();
#ifdef _OPENMP
    #pragma omp parallel for
#endif
    for (int i = 0; i < static_cast<int> (gNodes->size()); ++i)
    {
        Node* node = gNodes->at(i);
        Eigen::Vector3d nodePos = node->getCoordinates();

        double minAbsDist = std::numeric_limits<double>::max();
        double signedDist = 0.0;

        for (const Triangle& triangle : triangles)
        {
            const auto& v1 = triangle.getVertex1();
            const auto& v2 = triangle.getVertex2();
            const auto& v3 = triangle.getVertex3();
            const auto& n  = triangle.getNormal().normalized();

            double d = pointTriangleDistance(nodePos, v1, v2, v3);
            double sign = (nodePos - v1).dot(n) < 0.0 ? -1.0 : 1.0;
            double sd = sign * d;

            if (std::abs(d) < minAbsDist)
            {
                minAbsDist = std::abs(d);
                signedDist = sd;
            }
        }

        node->setDistanceLevelSet(signedDist);
    }
}

void TerrainContact::particleDistanceLevelSet(Mesh* mesh, vector<Particle*>* particles) {
    
	// get nodes
    vector<Node*>* nodes = mesh->getNodes();

    // iterate over all particles
    for (size_t i = 0; i < particles->size(); ++i) 
	{	
		// get the particle
        Particle* particle = particles->at(i);

        // only active particles are considered
        if (!particle->getActive()) { continue; }

        // get nodes and weights contributing to the particle
        const vector<Contribution>* contribution = particle->getContributionNodes();

        // initialize the interpolated distance level set value
        double interpolatedDistance = 0.0;

        // iterate over the nodal contribution list
        for (size_t j = 0; j < contribution->size(); ++j) {
            
			// get contributing node
            Node* nodeI = nodes->at(contribution->at(j).getNodeId());

            // get the nodal distance level set value
            double nodalDistance = nodeI->getDistanceLevelSet();

            // get the contribution weight
            double weight = contribution->at(j).getWeight();

            // accumulate the contribution
            interpolatedDistance += weight * nodalDistance;
        }

        // set interpolated value to particle
        particle->setDistanceLevelSet(interpolatedDistance);
    }
}

void TerrainContact::nodalDensityLevelSet(Mesh* mesh, vector<Particle*>* particles) 
{
	// get nodes
	vector<Node*>* nodes = mesh->getNodes();

	// for each particle
	for (size_t i = 0; i < particles->size(); ++i) {

		// only active particle can contribute
		if (!particles->at(i)->getActive()) { continue; }

		// get nodes and weights that the particle contributes
		const vector<Contribution>* contribution = particles->at(i)->getContributionNodes();

		// get the particle mass
		const double pVolume = particles->at(i)->getInitialVolume();

		// for each node in the contribution list 
		for (size_t j = 0; j < contribution->size(); ++j) {

			// get the contributing node
			Node* nodeI = nodes->at(contribution->at(j).getNodeId());

			// compute and set the weighted density level set
			nodeI->addDensityLevelSet(pVolume*contribution->at(j).getWeight()/nodeI->getVolume());
		}
	}
}

double interpolateDensityLevelSet(const Vector3d& position, const vector<int>& nodeIds, Mesh* mesh) {
    
    // get the nodes of the mesh
    vector<Node*>* gridNodes = mesh->getNodes();

	// get the nodes of the cell that contains the position
    Vector3d p0 = gridNodes->at(nodeIds[0])->getCoordinates();
    Vector3d p7 = gridNodes->at(nodeIds[7])->getCoordinates();

    // density level values of the nodes
    double d0 = gridNodes->at(nodeIds[0])->getDensityLevelSet();
    double d1 = gridNodes->at(nodeIds[1])->getDensityLevelSet();
    double d2 = gridNodes->at(nodeIds[2])->getDensityLevelSet();
    double d3 = gridNodes->at(nodeIds[3])->getDensityLevelSet();
    double d4 = gridNodes->at(nodeIds[4])->getDensityLevelSet();
    double d5 = gridNodes->at(nodeIds[5])->getDensityLevelSet();
    double d6 = gridNodes->at(nodeIds[6])->getDensityLevelSet();
    double d7 = gridNodes->at(nodeIds[7])->getDensityLevelSet();

/* 
       (6)------(7)  <- maxCorner = p7 (x_max, y_max, z_max)
       /|       /|
      / |      / |
    (4)------(5) |
     | (2)----|-(3)
     | /      | /
    (0)------(1)     <- minCorner = p0 (x_min, y_min, z_min)
*/

    // get the natural coordinates of the position
    
    // limit corners
    Vector3d minCorner = p0; // minimum corner (x_min,y_min,z_min)
    Vector3d maxCorner = p7; // maximum corner (x_max,y_max,z_max)

    // natural coordinates of the position
    double u = (2.0 * (position.x() - minCorner.x()) / (maxCorner.x() - minCorner.x())) - 1.0;
    double v = (2.0 * (position.y() - minCorner.y()) / (maxCorner.y() - minCorner.y())) - 1.0;
    double w = (2.0 * (position.z() - minCorner.z()) / (maxCorner.z() - minCorner.z())) - 1.0;

    // shape functions of the hexahedron
    double N0 = (1 - u) * (1 - v) * (1 - w) * 0.125;
    double N1 = (1 + u) * (1 - v) * (1 - w) * 0.125;
    double N2 = (1 - u) * (1 + v) * (1 - w) * 0.125;
    double N3 = (1 + u) * (1 + v) * (1 - w) * 0.125;
    double N4 = (1 - u) * (1 - v) * (1 + w) * 0.125;
    double N5 = (1 + u) * (1 - v) * (1 + w) * 0.125;
    double N6 = (1 - u) * (1 + v) * (1 + w) * 0.125;
    double N7 = (1 + u) * (1 + v) * (1 + w) * 0.125;

    // trilinear interpolation of the density level set
    double interpolatedDensity = N0 * d0 + N1 * d1 + N2 * d2 + N3 * d3 +
                                 N4 * d4 + N5 * d5 + N6 * d6 + N7 * d7;

    return interpolatedDensity;
}

void TerrainContact::trianglesDensityLevelSet(Mesh* mesh) {
    
    // get the triangles of the STL mesh
    const vector<Triangle>& triangles = this->getSTLMesh()->getTriangles();

    // create a vector to store the interpolated values
    vector<double> interpolatedValues(triangles.size(), 0.0);

    // iterate over all triangles
#ifdef _OPENMP
    #pragma omp parallel for shared(triangles, mesh, interpolatedValues)
#endif
    for (int i = 0; i < static_cast<int>(triangles.size()); ++i) {
        
        // get the current triangle
        const Triangle& triangle = triangles[i];

        // get the centroid of the triangle
        Vector3d centroid = triangle.getCentroid();

        // get the nodes of the cell that contains the centroid
        vector<int> nodeIds = mesh->getNodesInCell(centroid);

        // check if the cell is valid
        if (nodeIds.size() == 8) {
            // interpolate the density level set value to the centroid of the triangle
            interpolatedValues[i] = interpolateDensityLevelSet(centroid, nodeIds, mesh);
        } 
    }

    // store the interpolated values in the TerrainContact structure
    this->densityLevelSet = interpolatedValues;
}

void TerrainContact::determineContactPotentialPairs(Mesh* mesh, std::vector<Particle*>* particles) {

    // distance threshold for contact detection
    const double d_threshold = this->scalingFactor*(mesh->getCellDimension()).mean();

    // get the triangles and the density values
    const std::vector<Triangle>& triangles = stlMesh->getTriangles();
    const std::vector<double>& densityValues = this->densityLevelSet;

    // verify that the number of density values is equal to the number of triangles
    if (densityValues.size() != triangles.size()) {
        return;
    }

    // clear the contact pairs
    contactPairs.clear();

    // loop over the particles to determine the contact potential pairs
#ifdef _OPENMP
    #pragma omp parallel for shared(particles, triangles, densityValues)
#endif
    for (int i = 0; i < static_cast<int>(particles->size()); ++i) {
    
        // get the particle
        Particle* particle = particles->at(i);

        // get the distance level set value of the particle
        double d_p = particle->getDistanceLevelSet();

        // check if the particle is near the terrain contact (first condition for contact detection)
        if (std::abs(d_p) < d_threshold)
        {
            // find the closest triangle to the particle
            double minDistance = 1e+10;
            int closestTriangleIndex = -1;

            for (size_t j = 0; j < triangles.size(); ++j) {

                Vector3d centroid = triangles[j].getCentroid();
                double distance = (centroid - particle->getPosition()).norm();
                
                if (distance < minDistance) {
                    minDistance = distance;
                    closestTriangleIndex = static_cast<int>(j);
                }
            }

            // if the closest triangle is not found, continue
            if (closestTriangleIndex == -1) continue;

            // if the density value of the closest triangle is positive, add the pair to the contact pairs (second condition for contact detection)
            if (densityValues[closestTriangleIndex] > 0.0) {
#ifdef _OPENMP
                #pragma omp critical
#endif
                contactPairs.push_back(std::make_pair(particle, const_cast<Triangle*>(&triangles[closestTriangleIndex])));
            }
        }
    }
}

void TerrainContact::computeContactForces(double dt) {

    // check if seismic analysis is enabled
    bool isSeismic = ModelSetup::getSeismicAnalysisActive();

    // get the accumulated velocity from seismic analysis
    Vector3d v_surface = isSeismic ? Seismic::getAccumulatedVelocity() : Vector3d::Zero(); 

    // for all contact pairs
#ifdef _OPENMP
    #pragma omp parallel for shared(v_surface, isSeismic)
#endif
    for (int i = 0; i < static_cast<int>(contactPairs.size()); ++i) 
    {
        // get the particle and the triangle in contact
        Particle* particle = contactPairs[i].first;
        Triangle* triangle = contactPairs[i].second;

        // get the normal of the triangle
        Vector3d normal = triangle->getNormal().normalized();

        // get the mass and velocity of the particle
        double mass = particle->getMass();

        Vector3d velocityPredictor = isSeismic ? (particle->getVelocity() - v_surface) : particle->getVelocity();

        // calculate the normal velocity v_n = (v_p . e_n) e_n
        double vn_magnitude = velocityPredictor.dot(normal);

        // prevent non-penetrating contact correction
        if (vn_magnitude >= 0.0) continue;
        
        Vector3d vn = vn_magnitude * normal;

        // calculate the normal force f_n = -m_p * vn_p / dt * e_n
        Vector3d fn = - (mass * vn_magnitude / dt) * normal;
        
        // if penalty contact is enabled, apply the penalty force
        if (usePenaltyContact) {
            double penetration = -particle->getDistanceLevelSet();
            if (penetration > 0.0)
            {
                // calculate the penalty force f_penalty = k * penetration * e_n
                Vector3d f_penalty = penaltyStiffness * penetration * normal;
                // apply the penalty force to the normal force
                fn += f_penalty;
            }
        }
        
        // calculate tangential force f_t = -m_p (v_p - vn) / dt
        Vector3d ft = - (mass / dt) * (velocityPredictor - vn);

        // apply Coulomb friction ||f_t|| <= mu ||f_n||
        double fn_mag = fn.norm();
        double ft_mag = ft.norm();
        double mu =this->frictionCoefficient;

        if (ft_mag > mu * fn_mag) {
            // scale the tangential force to satisfy the Coulomb friction condition
            ft = (mu * fn_mag / ft_mag) * ft;
        }

        // calculate the corrected velocity v_p^* = v_p + dt (f_n + f_t) / m_p
        Vector3d velocityCorrected = velocityPredictor + (dt / mass) * (fn + ft);

        if (isSeismic) {
            // if seismic analysis is enabled, add the surface velocity
            velocityCorrected += v_surface;
        }

        // update the velocity of the particle
        particle->setVelocity(velocityCorrected);
    }
}

void TerrainContact::apply(Mesh* mesh, std::vector<Particle*>* particles, double dt)
{
    // calculate the distance level set function to particles
    particleDistanceLevelSet(mesh, particles);

    // calculate the nodal density level-set
    nodalDensityLevelSet(mesh, particles);

    // interpolate the density at the center of triangles
    trianglesDensityLevelSet(mesh);

    // determine the contact potential pairs
    determineContactPotentialPairs(mesh, particles);

    // compute the contact forces and correct velocities
    computeContactForces(dt);
}

void TerrainContact::enablePenaltyContact(bool enable) {
    usePenaltyContact = enable;
}

void TerrainContact::setPenaltyStiffness(double k) {
    penaltyStiffness = k;
}