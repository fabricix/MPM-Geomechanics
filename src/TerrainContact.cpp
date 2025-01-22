
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