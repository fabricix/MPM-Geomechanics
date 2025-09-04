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
    ContactManager(double _friction, double _master_id, double _slave_id, string _normal_type, double _real_distance_correction_coefficient)
        :frictionCoefficient(_friction) {
        if (_master_id == NULL) {
            masterId = 0;
        }
        else {
            masterId = _master_id;
        }

        if (_slave_id == NULL) {
            slaveId = 1;
        }
        else {
            slaveId = _slave_id;
        }
        
        if (_normal_type != "collinear" and _normal_type != "slave") {
            normalType = "master";
        }
        else {
            normalType = _normal_type;
        }

        if (_real_distance_correction_coefficient == NULL) {
            realDistanceCorrectionActive = false;
        }
        else {
            realDistanceCorrectionActive = true;
            realDistanceCorrectionCoefficient = _real_distance_correction_coefficient;
        }
    }

    /// \brief Clear all contact node data
    void clear();

    /// \brief Compute contact forces (normal and tangential) for all contact nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of Body pointers
	/// \param[in] time_step Time step
    void computeContactForces(Mesh* mesh, vector<Body*>* bodies, double dt);

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
