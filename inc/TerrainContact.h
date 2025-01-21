#ifndef TERRAIN_CONTACT_H
#define TERRAIN_CONTACT_H

#include "Mesh/STLReader.h"

/// @brief Class to compute terrain contact
class TerrainContact {
private:
    
    double frictionCoefficient; ///< Friction coefficient \f$\mu\f$
    STLReader* stlMesh; //!< triangular mesh for terrain contact

public:

    TerrainContact(double friction)
        : frictionCoefficient(friction) {}

    /// @brief set the stl mesh
    /// @param mesh STL mesh pointer
    void setSTLMesh(STLReader* mesh) {
        stlMesh = mesh;
    }

    /// @brief set the friction coefficient
    /// @param friction Friction coefficient \f$\mu\f$
    void setFrictionCoefficient(double friction) {
        frictionCoefficient = friction;
    }
};

#endif