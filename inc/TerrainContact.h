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

public:

    TerrainContact( STLReader* mesh, double friction)
        : stlMesh(mesh), frictionCoefficient(friction) {}

    /// @brief compute the distance level set function in nodes \f$ d_{I}=(X_I-X_i)e_n \f$
    void computeDistanceLevelSetFunction(Mesh* mesh);

    /// \brief Interpolate distance level set function value of particle
	/// \f$ d_p = \sum_I d_I N_{Ip} \f$
	void particleDistanceLevelSet(Mesh* mesh, vector< Particle* >* particles);

	/// \brief Calculate the nodal distance level set function
	/// \f$ \rho_I = \sum_p \frac{V_p N_{Ip}}{V_I} \f$
	void nodalDistanceLevelSet(Mesh* mesh, vector< Particle* >* particles);

	/// \brief Interpolate the density level set function in the centroids of triangles 
    /// \f$ \rho_m = \sum_I \rho_I N_{Im} \f$
	void trianglesDensityLevelSet(Mesh* mesh);

    /// @brief Set the interpolated density level set function in the centroids of triangles
    void setDensityLevelSet(const std::vector<double>& density) { densityLevelSet = density; }

    /// @brief Get the triangular mesh
    STLReader* getSTLMesh() { return stlMesh; }
};

#endif