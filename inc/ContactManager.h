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

    /// \brief Add mass and momentum to a contact node
    void addMassAndMomentum(int nodeId, int body, double mass, const Vector3d& momentum);

    /// \brief Add internal and external forces to a contact node
    void addForces(int nodeId, int body, const Vector3d& internalForce, const Vector3d& externalForce);

    /// \brief Compute contact forces (normal and tangential) for all contact nodes
    static void computeContactForces(Mesh* mesh, vector<Body*>* bodies, double dt);

    /// \brief Apply contact momentum corrections to the nodes
    void applyContactCorrections(std::vector<Node*>& nodes, double dt);

    /// \brief Check if a node has contact data
    bool hasContact(int nodeId) const;

    /// \brief Calculate the normal vector to the node
    ///
    /// \f$ m_I = \sum_p m_p N_{Ip}\f$
    ///
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    static void nodalUnitNormal(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check nodes in contact
    void contactCheck(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check nodes in contact
    static void contactCheckCorrection(Mesh* mesh, vector<Body*>* bodies);

    /*struct ContactNodeData {
        int nodeId = -1;
        int bodyMasterId = -1;
        int bodySlaveId = -1;
        double mu = 0.0;

        double massMaster = 0.0;
        double massSlave = 0.0;
        Vector3d momentumMaster = Vector3d::Zero();
        Vector3d momentumSlave = Vector3d::Zero();
        Vector3d internalForceMaster = Vector3d::Zero();
        Vector3d internalForceSlave = Vector3d::Zero();
        Vector3d externalForceMaster = Vector3d::Zero();
        Vector3d externalForceSlave = Vector3d::Zero();
        Vector3d contactForce = Vector3d::Zero();
        Vector3d normalContactForce = Vector3d::Zero();
        Vector3d tangentialContactForce = Vector3d::Zero();
        Vector3d normal = Vector3d::Zero();
    };*/

    
    /// \brief Get unordered_map contactNodes
    const std::unordered_map<int, Mesh::ContactNodeData>& getContactNodes() const{return contactNodes;}
    

private:
   
    std::unordered_map<int, Mesh::ContactNodeData> contactNodes;
};

#endif /* CONTACT_MANAGER_H_ */
