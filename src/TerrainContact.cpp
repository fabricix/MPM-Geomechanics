
#include "TerrainContact.h"
#include "Mesh/Mesh.h"
#include "limits"

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

void TerrainContact::interpolateDensityLevelSetTriangles(Mesh* mesh, TerrainContact* terrainContact) {
    
    // get the triangles of the STL mesh
    const vector<Triangle>& triangles = terrainContact->getSTLMesh()->getTriangles();

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
    terrainContact->setDensityLevelSet(interpolatedValues);
}