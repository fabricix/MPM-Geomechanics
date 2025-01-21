#ifndef TERRAIN_CONTACT_H
#define TERRAIN_CONTACT_H

#include "Mesh/STLReader.h"
#include "Mesh/Mesh.h"

/// @brief Class to compute terrain contact
class TerrainContact {
private:
    
    double frictionCoefficient; ///< Friction coefficient \f$\mu\f$
    STLReader* stlMesh; //!< triangular mesh for terrain contact

public:

    TerrainContact( STLReader* mesh, double friction)
        : stlMesh(mesh), frictionCoefficient(friction) {}

    /// @brief compute the distance level set function \f$ d_{I}=(X_I-X_i)e_n \f$
    void computeDistanceLevelSetFunction(Mesh* mesh);
};

#endif