/*
 * NodeContact.h
 *
 *  Created on: 11 de dez de 2024
 *      Author: Matheus Peres <matheuslperes@tmail.com>
 */

#ifndef INC_MESH_NODECONTACT_H_
#define INC_MESH_NODECONTACT_H_

#include <Mesh/Node.h>

/// \class NodeMixture
/// \brief Represents a mesh node for calculations using the mixture theory
class NodeContact: public Node {

public:

    /// \brief Default constructor
    ///
    NodeContact();

    /// \brief Default destructor
    ///
    virtual ~NodeContact();

    /// \brief Delete all values stored in node
    ///
    virtual void resetValues();

    /// \brief Add mass increment to the slave nodal
    /// \param[in] slave_mass_increment Nodal mass increment 
    virtual inline void addMassSlave(double slave_mass_increment) { this->massSlave += slave_mass_increment; }

    /// \brief Add momentum increment to the slave nodal momentum
    /// \param[in] slave_momentum_increment Vector containing the nodal momentum increment
    virtual inline void addMomentumSlave(const Vector3d& slave_momentum_increment) { this->momentumSlave += slave_momentum_increment; }

    /// \brief Add a internal force increment to the slave nodal internal force
    /// \param[in] internal_force_slave_increment Vector containing nodal internal force increment
    virtual inline void addInternalForceSlave(const Vector3d& internal_force_slave_increment) { this->internalForceSlave += internal_force_slave_increment; }

    /// \brief Add a external force increment to the slave nodal external force
    /// \param[in] external_force_slave_increment Vector containing nodal external force increment 
    virtual inline void addExternalForceSlave(const Vector3d& external_force_slave_increment) { this->externalForceSlave += external_force_slave_increment; }

    /// \brief Configure the slave nodal momentum
    /// \param[in] nodal_momentum_slave Vector containing the nodal momentum
    virtual inline void setMomentumSlave(const Vector3d& nodal_momentum_slave) { this->momentumSlave = nodal_momentum_slave; }

    /// \brief Return the slave nodal momentum
    /// \return Vector containing the slave nodal momentum
    virtual inline const Vector3d* getMomentumSlave() const { return &(this->momentumSlave); }

    /// \brief Calculate the total nodal force
    ///
    virtual void updateTotalForce();

    /// \brief Calculate the damping nodal force
    ///
    virtual void updateDampingForce();

    /// \brief Configure the total slave nodal force
    /// \param[in] total_nodal_force_slave Vector containing the total nodal force 
    virtual inline void setTotalForceSlave(const Vector3d& total_nodal_force_slave) { this->totalForceSlave = total_nodal_force_slave; }

    /// \brief Return the slave nodal total force
    /// \return Vector containing the slave nodal total force
    virtual inline const Vector3d* getTotalForceSlave() const { return &(this->totalForceSlave); }

    /// brief Integrate momentum
    ///
    virtual void integrateMomentum(double dt);

    /// \brief Return the slave nodal mass
    /// \return slave Nodal mass
    virtual inline double getMassSlave() const { return massSlave; }

    /// \brief Update nodal velocity of mixture
    ///
    virtual void updateVelocity();

    /// \brief Return the slave nodal velocity
    /// \return Vector containing the slave nodal velocity
    virtual inline const Vector3d* getVelocitySlave() const { return &(this->velocitySlave); }

    /// \brief Configure the has contact flag
    /// Has contact flag
    virtual inline void setContactFlag(bool flag) { this->hasContact = flag; }

    /// \brief Return the contact status
    /// \return contact status
    virtual inline const bool getContactStatus() const { return (this->hasContact); }

private:

    double massSlave; //!< nodal mass: \f$m_{I}\f$, or solid mass in two-phase calculations: \f$m_{I}^{s}\f$

    Vector3d momentumSlave; //!< nodal momentum: \f$p_{iI}\f$, or momentum in solid in two-phase calculations: \f$p_{iI}^{s}\f$
    Vector3d velocitySlave; //!< nodal velocity: \f$v_{iI}\f$, or velocity in solid in two-phase calculations: \f$v_{iI}^{s}\f$
    Vector3d externalForceSlave; //!< nodal external force: \f$f_{iI}^{ext}\f$, or external force in solid in two-phase calculations: \f$f_{iI}^{ext,s}\f$
    Vector3d internalForceSlave; //!< nodal internal force: \f$f_{iI}^{int}\f$, or internal force in solid in two-phase calculations: \f$f_{iI}^{int,s}\f$
    Vector3d dampingForceSlave; //!< nodal damping force: \f$f_{iI}^{dmp}\f$, or damping force in solid in two-phase calculations: \f$f_{iI}^{dmp,s}\f$
    Vector3d totalForceSlave; //!< nodal total force: \f$f_{iI}\f$, or total force in solid in two-phase calculations: \f$f_{iI}^{s}\f$
    bool hasContact; //!< contact flag
};

inline NodeContact::NodeContact()
{
    massSlave = 0;
    momentumSlave.setZero();
    velocitySlave.setZero();
    externalForceSlave.setZero();
    internalForceSlave.setZero();
    dampingForceSlave.setZero();
    totalForceSlave.setZero();
    hasContact = false;
}

inline void NodeContact::resetValues()
{
    Node::resetValues();

    massSlave = 0.0;
    momentumSlave.setZero();
    externalForceSlave.setZero();
    internalForceSlave.setZero();
}

#endif /* INC_MESH_NODECONTACT_H_ */