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
/// \brief Manages conventional contact method data and operations
/// This class contain all variables and functions to manage
/// the conventional contact method using two velocity fields.
class ContactManager {
public:
    /// @brief  Constructor
    /// @param friction Friction coefficient
    /// @param master_id ID of the master body
    /// @param slave_id ID of the slave body
    /// @param normal_type Normal type: "master", "slave" or "collinear"
    /// @param real_distance_correction_coefficient Real distance correction coefficient (0 to disable)
    ContactManager(double friction, int master_id, int slave_id, string normal_type, double real_distance_correction_coefficient);

    /// \brief Clear all contact node data
    void clear();

    /// \brief Compute contact forces (normal and tangential) for all contact nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of Body pointers
	/// \param[in] time_step Time step
    void computeContactForces(Mesh* mesh, double dt);

    /// \brief Calculate the normal vector to the node
    ///
    /// \f$ n_I = \frac{\sum_p m_p N_{Ip}}{\left| \sum_p m_p N_{Ip} \right|} \f$
    ///
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void nodalUnitNormal(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check nodes in contact
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void contactCheck(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check nodes in contact
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void realDistanceCorrection(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Update the nodal momentum after contact forces calculation
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies List of Body pointers
    /// \param[in] time_step Time step
    void nodalMomentumContactUpdate(Mesh* mesh, vector<Body*>* bodies, double dt);

    /// \brief Update the nodal momentum after contact forces calculation
    /// \param[in] mesh Mesh reference
    /// \param[in] time_step Time step
    void nodalMomentumCorrection(Mesh* mesh, double dt);

    /// \brief Get unordered_map contactNodes
    //const std::unordered_map<int, Mesh::ContactNodeData>& getContactNodes() const{return contactNodes;}

private:
   
    //std::unordered_map<int, Mesh::ContactNodeData> contactNodes;
    string normalType; //Default: master
    double frictionCoefficient; //!< Friction coefficient \f$\mu\f$
    bool realDistanceCorrectionActive;
    double realDistanceCorrectionCoefficient;
    bool hasContact;
    int masterId; 
    int slaveId;
};

#endif /* CONTACT_MANAGER_H_ */
