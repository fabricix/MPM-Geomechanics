// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CONTACT_MANAGER_H_
#define CONTACT_MANAGER_H_

#include "Eigen/Core"
using Eigen::Vector3d;

#include "Mesh/Node.h"
#include "Mesh/Mesh.h"
#include "Body/Body.h"
#include "Contact.h"

#include <unordered_map>
#include <vector>
using std::vector;

/// \class ContactManager
/// \brief Manages Slave-Master contact method data and operations using two velocity fields.
class ContactManager {
public:
    /// @brief  Constructor
    /// @param contact list
    /// @param real_distance_correction_coefficient Real distance correction coefficient (0 to disable)
    ContactManager(vector<Contact*> contact_List, double real_distance_correction_coefficient);

    /// \brief Compute nodal contact for sticking and for non sticking contact.
	/// \param[in] time_step Time step
    void computeContactForces(Mesh* mesh, double time_step);

    /// \brief Calculate the nodal normal mass vector \f$ n_I = \frac{\sum_p m_p N_{Ip}}{\left| \sum_p m_p N_{Ip} \right|} \f$
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void nodalUnitNormal(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Check if nodes are in contact
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void contactCheck(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Applies the "real distance correction" method for contact detection.
    ///  \f$ D^{ MS }_I = - X_I ^ M n_I ^ M - X_I ^ S n_I ^ S \leq \lambda \, d_c\f$
    /// \param[in] mesh Mesh reference
    /// \param[in] bodies A list of Bodies
    void realDistanceCorrection(Mesh* mesh, vector<Body*>* bodies);

    /// \brief Update the nodal momentum after contact forces calculation  \f$ p^{ b,k + \frac{1}{2}}_{\, iI} = \bar{ p } ^ { b,k + \frac{1}{2} }_{\, iI} + \Delta t^k f^{ b,c,k }_{\, iI}\f$
    /// \param[in] mesh Mesh reference
    /// \param[in] time_step Time step
    void nodalMomentumContactUpdate(Mesh* mesh, double time_step);

    /// \brief Return if the real distance correction is active
    /// \return real distance correction active
    inline bool getRealDistanceCorrectionFlag() const { return this->realDistanceCorrectionActive; }

    bool contactDetection = false; //!<variable for contact detection

private:
    vector<Contact*> contactList; 
    double realDistanceCorrectionCoefficient = 0.0; //!< Real distance correction coefficient \f$\lambda\f$
    bool realDistanceCorrectionActive = false; //!< Real distance correction active
};

#endif /* CONTACT_MANAGER_H_ */
