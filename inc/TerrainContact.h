// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef TERRAIN_CONTACT_H
#define TERRAIN_CONTACT_H

#include "Mesh/STLReader.h"
#include "Mesh/Mesh.h"

/// @brief Class to compute terrain contact
class TerrainContact {

private:
    
    STLReader* stlMesh; //!< triangular mesh for terrain contact 
    
    double frictionCoefficient; //!< Friction coefficient \f$\mu\f$

    std::vector<double> densityLevelSet; //!< density level set function interpolated in centroids of triangles \f$ \rho_{m} \f$

    std::vector<std::pair<Particle*, Triangle*>> contactPairs; //!< contact potential pairs

    double scalingFactor = 2.0; //!< scaling factor for the distance threshold in contact detection \f$ \alpha \f$

    bool usePenaltyContact = false; //!< Use penalty contact method

    double penaltyStiffness = 0.0; //!< Penalty stiffness for contact forces

public:

    TerrainContact(STLReader* mesh, double friction)
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

    // \brief Compute the contact forces
    // \f$ f_n = -m_p \frac{v_p^n}{dt} e_n \f$
    // \f$ f_t = -m_p \frac{v_p - v_p^n e_n}{dt} \f$
    void computeContactForces(double dt);

    // \brief Project particles that have penetrated the STL mesh
    // If a particle is inside the STL mesh, move it to the surface
    // \f$ x_projected = x_p - d * e_n \f$
    void projectParticles(Mesh *mesh, std::vector<Particle *> *particles);

    /// @brief  Set  the distance threshold for contact detection
    /// @param threshold 
    void setDistanceThreshold(double threshold) { scalingFactor = threshold > 0.0 ? threshold : 2.0; };

    /// @brief Apply the terrain contact algorithm
    void apply(Mesh* mesh, std::vector<Particle*>* particles, double dt);

    /// @brief Enable the penalty contact method
    void enablePenaltyContact(bool enable);
    
    /// @brief Set the penalty stiffness for contact forces
    void setPenaltyStiffness(double k);
};

#endif // TERRAINCONTACT_H