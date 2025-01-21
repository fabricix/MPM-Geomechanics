
#include "TerrainContact.h"
#include "Mesh/Mesh.h"
#include "limits"

void TerrainContact::computeDistanceLevelSetFunction(Mesh* mesh)
{
    // get background grid nodes
    vector<Node*>* gNodes = mesh->getNodes();

    // get triangles from stl mesh
    const vector<Triangle>& triangles = stlMesh->getTriangles();

    // iterate over all nodes in the background mesh
    #pragma omp parallel for shared(gNodes, triangles)
    for (int i = 0; i < gNodes->size(); ++i)
    {
        // get node
        Node* node = gNodes->at(i);

        // get node position
        Vector3d nodePosition = node->getCoordinates();

        // variable to store the minimum distance
        double minDistance = 1e6;

        // variable to check if all distances are negative
        bool allNegative = true;

        // iterate over all triangles in the stl mesh
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

            // Check if at least one distance is positive
            if (d1 > 0.0 || d2 > 0.0 || d3 > 0.0) {
                allNegative = false;
            }

            // Consider only positive distances for the minimum calculation
            if (d1 > 0.0) {
                minDistance = std::min(minDistance, d1);
            }
            if (d2 > 0.0) {
                minDistance = std::min(minDistance, d2);
            }
            if (d3 > 0.0) {
                minDistance = std::min(minDistance, d3);
            }
        }

        // if all distances for all triangles are negative, set minDistance to -1
        if (allNegative) {
            minDistance = -1.0;
        }

        // store the minimum distance in the node
        node->setDistanceLevelSet(minDistance);
    }
}