
#include "TerrainContact.h"
#include "Mesh/Mesh.h"
#include "limits"
#include "Contribution.h"
#include "Particle/Particle.h"

#include <utility>  // std::pair

void TerrainContact::computeDistanceLevelSetFunction(Mesh* mesh)
{
    // get background grid nodes
    vector<Node*>* gNodes = mesh->getNodes();

    // get triangles
    const vector<Triangle>& triangles = stlMesh->getTriangles();

    // iterate over all nodes in the background mesh
    #pragma omp parallel for shared(gNodes, triangles)
    for (int i = 0; i < gNodes->size(); ++i)
    {
        // get node
        Node* node = gNodes->at(i);

        // get node position
        Vector3d nodePosition = node->getCoordinates();

        // variable to store the minimum positive distance
        double minPositiveDistance = 1e6;

        // variable to store the maximum negative distance
        double maxNegativeDistance = -1e6;

        // iterate over all triangles in the STL mesh
        for (const Triangle& triangle : triangles)
        {
            // get vertices of the triangle
            Vector3d v1 = triangle.getVertex1();
            Vector3d v2 = triangle.getVertex2();
            Vector3d v3 = triangle.getVertex3();
            Vector3d normal = triangle.getNormal();
            
            // calculate the distance from the node to each vertex
            double d1 = (nodePosition - v1).dot(normal);
            double d2 = (nodePosition - v2).dot(normal);
            double d3 = (nodePosition - v3).dot(normal);

            // update the minimum positive distance
            if (d1 > 0.0) {
                minPositiveDistance = std::min(minPositiveDistance, d1);
            } else {
                maxNegativeDistance = std::max(maxNegativeDistance, d1);
            }
            if (d2 > 0.0) {
                minPositiveDistance = std::min(minPositiveDistance, d2);
            } else {
                maxNegativeDistance = std::max(maxNegativeDistance, d2);
            }
            if (d3 > 0.0) {
                minPositiveDistance = std::min(minPositiveDistance, d3);
            } else {
                maxNegativeDistance = std::max(maxNegativeDistance, d3);
            }
        }

        // decide which distance to store in the node
        double finalDistance = (minPositiveDistance < 1e6) ? minPositiveDistance : maxNegativeDistance;

        // store the distance in the node
        node->setDistanceLevelSet(finalDistance);
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
    Vector3d p1 = gridNodes->at(nodeIds[1])->getCoordinates();
    Vector3d p2 = gridNodes->at(nodeIds[2])->getCoordinates();
    Vector3d p3 = gridNodes->at(nodeIds[3])->getCoordinates();
    Vector3d p4 = gridNodes->at(nodeIds[4])->getCoordinates();
    Vector3d p5 = gridNodes->at(nodeIds[5])->getCoordinates();
    Vector3d p6 = gridNodes->at(nodeIds[6])->getCoordinates();
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
    #pragma omp parallel for shared(triangles, mesh, interpolatedValues)
    for (int i = 0; i < triangles.size(); ++i) {
        
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
    const double d_threshold = 2.0*(mesh->getCellDimension()).mean();

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
    #pragma omp parallel for shared(particles, triangles, densityValues)
    for (int i = 0; i < particles->size(); ++i) {
    
        // get the particle
        Particle* particle = particles->at(i);

        // get the distance level set value of the particle
        double d_p = particle->getDistanceLevelSet();

        // check if the particle is near the terrain contact (first condition for contact detection)
        if ( d_p < d_threshold && d_p > 0.0)
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
                #pragma omp critical
                contactPairs.push_back(std::make_pair(particle, const_cast<Triangle*>(&triangles[closestTriangleIndex])));
            }
        }
    }
}

void TerrainContact::computeContactForces(std::vector< Particle* >* particles, double dt) {
    
    // for all contact pairs
    #pragma omp parallel for shared(particles)
    for (int i = 0; i < contactPairs.size(); ++i) 
    {
        // get the particle and the triangle in contact
        Particle* particle = contactPairs[i].first;
        Triangle* triangle = contactPairs[i].second;

        // get the normal of the triangle
        Vector3d normal = triangle->getNormal().normalized();

        // get the mass and velocity of the particle
        double mass = particle->getMass();
        Vector3d velocityPredictor = particle->getVelocity();

        // calculate the normal velocity v_n = (v_p . e_n) e_n
        double vn_magnitude = velocityPredictor.dot(normal);
        Vector3d vn = vn_magnitude * normal;

        // calculate the normal force f_n = -m_p * vn_p / dt * e_n
        Vector3d fn = - (mass * vn_magnitude / dt) * normal;

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

        // update the velocity of the particle
        particle->setVelocity(velocityCorrected);
    }
}