// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CONTACT_MANAGER_H_
#define CONTACT_MANAGER_H_

#include <unordered_map>
#include <vector>
#include "Eigen/Core"
#include "Mesh/Node.h"
#include "Mesh/Mesh.h"
#include "Body/Body.h"

using Eigen::Vector3d;

/// \class ContactManager
/// \brief Manages contact data and calculations for MPM nodes
class ContactManager {
public:
    ContactManager() = default;

    /// \brief Clear all contact node data
    void clear();

    /// \brief Compute contact forces (normal and tangential) for all contact nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of Body pointers
	/// \param[in] time_step Time step
    static void computeContactForces(Mesh* mesh, vector<Body*>* bodies, double dt);

    /// \brief Calculate the normal vector to the node
    ///
    /// \f$ n_I = \frac{\sum_p m_p N_{Ip}}{\left| \sum_p m_p N_{Ip} \right|} \f$
    ///
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    static void nodalUnitNormal(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check nodes in contact
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void contactCheck(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check nodes in contact
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    static void contactCheckCorrection(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Get unordered_map contactNodes
    const std::unordered_map<int, Mesh::ContactNodeData>& getContactNodes() const{return contactNodes;}

private:
   
    std::unordered_map<int, Mesh::ContactNodeData> contactNodes;
};

#endif /* CONTACT_MANAGER_H_ */
