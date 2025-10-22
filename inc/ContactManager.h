// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CONTACT_MANAGER_H_
#define CONTACT_MANAGER_H_

#include "Eigen/Core"
#include "Mesh/Node.h"
#include "Mesh/Mesh.h"
#include "Body/Body.h"

#include <unordered_map>
#include <vector>

using Eigen::Vector3d;

/// \class ContactManager
/// \brief Manages Slave-Master contact method data and operations
/// This class contain all variables and functions to manage
/// the Slave-Master contact method using two velocity fields.
class ContactManager {
public:
    /// @brief  Constructor
    /// @param friction Friction coefficient
    /// @param master_id ID of the master body
    /// @param slave_id ID of the slave body
    /// @param normal_type Normal type: "master", "slave" or "collinear"
    /// @param real_distance_correction_coefficient Real distance correction coefficient (0 to disable)
    ContactManager(double friction, int master_id, int slave_id, string normal_type, double real_distance_correction_coefficient);

    /// \brief For all contact nodes, compute contact forces for sticking contact \f$ \left( f^{M,c,k}_{iI} = \frac{1} {\left(m^{M,k}_{I} + m^{S,k}_{I}  \right)\Delta \ t^k} \left( m^{M,k}_{I} \bar{p}^{S,k+\frac{1}{2}}_{iI} - m^{S,k}_{I} \bar{p}^{M,k+\frac{1}{2}}_{iI} \right) \right)\f$ 
    /// and, the normal \f$ \left( f^{M,nor,k}_{iI} = f^{M,c,k}_{jI} n^{M,k}_{jI} n^{M,k}_{iI} \right)\f$ and tangential \f$ \left( \min \Big( \|f^{M,tan,k}_{iI}\|, \mu \ \| f^{M,nor,k}_{iI}\| \Big) \frac{f^{M,tan,k}_{iI}}{ \|f^{M,tan,k}_{iI}\|} \right) \f$ componentes for non sticking contact
	/// \param[in] time_step Time step
    void computeContactForces(Mesh* mesh, double time_step);

    /// \brief Calculate the normal vector to the node \f$ n_I = \frac{\sum_p m_p N_{Ip}}{\left| \sum_p m_p N_{Ip} \right|} \f$
    ///
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void nodalUnitNormal(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check nodes in contact
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void contactCheck(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Applies real distance correction \f$ D^{ MS }_I = -X_I ^ M \cdot n_I ^ M - X_I ^ S \cdot n_I ^ S \leq \lambda \, d_c\f$ to verify contact between bodies
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void realDistanceCorrection(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Call nodalUnitNormal, computeContactForces and nodalMomentumCorrection to Update the nodal momentum if contact detected
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies List of Body pointers
    /// \param[in] time_step Time step
    void nodalMomentumContactUpdate(Mesh* mesh, vector<Body*>* bodies, double time_step);

    /// \brief Correct the nodal momentum after contact forces calculation  \f$ p^{ b,k + \frac{1}{2}}_{\, iI} = \bar{ p } ^ { b,k + \frac{1}{2} }_{\, iI} + \Delta t^k f^{ b,c,k }_{\, iI}\f$
    /// \param[in] mesh Mesh reference
    /// \param[in] time_step Time step
    void nodalMomentumCorrection(Mesh* mesh, double time_step);

private:
    double frictionCoefficient = 0.0; //!< Friction coefficient \f$\mu\f$
    int masterId = 0; //<! Master body id
    int slaveId = 1; //<! Slave body id
    string normalType = ""; //!< Normal type: "master", "slave" or "collinear"
    double realDistanceCorrectionCoefficient = 0.0; //!< Real distance correction coefficient \f$\lambda\f$
    bool realDistanceCorrectionActive = false; //!< Real distance correction active
};

#endif /* CONTACT_MANAGER_H_ */
