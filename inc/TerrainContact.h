#ifndef TERRAIN_CONTACT_H
#define TERRAIN_CONTACT_H

#include "Mesh/STLReader.h"
#include "Mesh/Mesh.h"

/// @brief Class to compute terrain contact
class TerrainContact {
private:
    
    double frictionCoefficient; ///< Friction coefficient \f$\mu\f$

    STLReader* stlMesh; //!< triangular mesh for terrain contact

    std::vector<double> densityLevelSet; //!< density level set function interpolated in centroids of triangles \f$ \rho_{m} \f$

    std::vector<std::pair<Particle*, Triangle*>> contactPairs; //!< contact potential pairs

public:

    TerrainContact( STLReader* mesh, double friction)
        : stlMesh(mesh), frictionCoefficient(friction) {}

    /// @brief compute the distance level set function in nodes \f$ d_{I}=(X_I-X_i) e_n \f$
    /// It is the distance from the node to the STL mesh
    void computeDistanceLevelSetFunction(Mesh* mesh);

    /// \brief Interpolate distance level set function value of particle
	/// \f$ d_p = \sum_I d_I N_{Ip} \f$
	void particleDistanceLevelSet(Mesh* mesh, vector< Particle* >* particles);

	/// \brief Calculate the nodal density level set function
	/// \f$ \rho_I = \sum_p \frac{V_p N_{Ip}}{V_I} \f$
    /// It allows to know the boundary of the body for a certain scalar
	void nodalDensityLevelSet(Mesh* mesh, vector< Particle* >* particles);

	/// \brief Interpolate the density level set function in the centroids of triangles 
    /// \f$ \rho_m = \sum_I \rho_I N_{Im} \f$
    /// It allows to know the boundary of the body in contact with the STL mesh
	void trianglesDensityLevelSet(Mesh* mesh);

    /// \brief Determine the contact potential pairs
    void determineContactPotentialPairs(Mesh* mesh, std::vector< Particle* >* particles);

    /// @brief Get the triangular mesh
    STLReader* getSTLMesh() { return stlMesh; }
};

#endif