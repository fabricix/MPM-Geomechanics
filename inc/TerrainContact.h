#ifndef TERRAIN_CONTACT_H
#define TERRAIN_CONTACT_H

#include "Mesh/STLReader.h"
#include "Mesh/Mesh.h"

/// @brief Class to compute terrain contact
class TerrainContact {
private:
    
    double frictionCoefficient; ///< Friction coefficient \f$\mu\f$

    STLReader* stlMesh; //!< triangular mesh for terrain contact

    std::vector<double> densityLevelSet; //!< density level set function interpolated in centroids of triangles

public:

    TerrainContact( STLReader* mesh, double friction)
        : stlMesh(mesh), frictionCoefficient(friction) {}

    /// @brief compute the distance level set function in nodes \f$ d_{I}=(X_I-X_i)e_n \f$
    void computeDistanceLevelSetFunction(Mesh* mesh);

    /// @brief compute the density level set function in the centroid of triangles
    void interpolateDensityLevelSetTriangles(Mesh* mesh, TerrainContact* terrainContact);

    /// @brief Get the triangular mesh
    STLReader* getSTLMesh() { return stlMesh; }

    /// @brief Set the interpolated density level set function in the centroids of triangles
    void setDensityLevelSet(const std::vector<double>& density) { densityLevelSet = density; }
};

#endif