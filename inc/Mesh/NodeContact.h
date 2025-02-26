/*
 * NodeContact.h
 *
 *  Created on: 11 de dez de 2024
 *      Author: Matheus Peres <matheuslperes@tmail.com>
 */

#ifndef INC_MESH_NODECONTACT_H_
#define INC_MESH_NODECONTACT_H_

#include <Mesh/Node.h>
#include <Particle/Particle.h>

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
    virtual inline void addMassSlave(double slave_mass_increment) { this->massSlaveBody += slave_mass_increment; }

    /// \brief Add momentum increment to the slave nodal momentum
    /// \param[in] slave_momentum_increment Vector containing the nodal momentum increment
    virtual inline void addMomentumSlave(const Vector3d& slave_momentum_increment) { this->momentumSlaveBody += slave_momentum_increment; }

    /// \brief Add nodal mass gradient increment
    /// \param[in] mass_gradient_increment Nodal mass gradient increment 
    virtual inline void addMassGradient(const Vector3d& mass_gradient_increment) { this->normal += mass_gradient_increment; }

    /// \brief Add slave nodal mass gradient increment
    /// \param[in] slave_mass_gradient_increment Nodal mass gradient increment 
    virtual inline void addMassGradientSlave(const Vector3d& slave_mass_gradient_increment) { this->normalSlave += slave_mass_gradient_increment; }

    /// \brief Add a internal force increment to the slave nodal internal force
    /// \param[in] internal_force_slave_increment Vector containing nodal internal force increment
    virtual inline void addInternalForceSlave(const Vector3d& internal_force_slave_increment) { this->internalForceSlaveBody += internal_force_slave_increment; }

    /// \brief Add a external force increment to the slave nodal external force
    /// \param[in] external_force_slave_increment Vector containing nodal external force increment 
    virtual inline void addExternalForceSlave(const Vector3d& external_force_slave_increment) { this->externalForceSlaveBody += external_force_slave_increment; }

    /// \brief Add a contact force to nodal total force  
    virtual inline void addContactForce() { this->totalForce += this->contactForce, this->totalForceSlaveBody -= this->contactForce; }

    /// \brief Configure the slave nodal momentum
    /// \param[in] nodal_momentum_slave Vector containing the nodal momentum
    virtual inline void setMomentumSlave(const Vector3d& nodal_momentum_slave) { this->momentumSlaveBody = nodal_momentum_slave; }

    /// \brief Return the slave nodal momentum
    /// \return Vector containing the slave nodal momentum
    virtual inline const Vector3d* getMomentumSlave() const { return &(this->momentumSlaveBody); }

    /// \brief Calculate the total nodal force
    ///
    virtual void updateTotalForce();

    /// \brief Calculate the damping nodal force
    ///
    virtual void updateDampingForce();

    /// \brief Configure the total slave nodal force
    /// \param[in] total_nodal_force_slave Vector containing the total nodal force 
    virtual inline void setTotalForceSlave(const Vector3d& total_nodal_force_slave) { this->totalForceSlaveBody = total_nodal_force_slave; }

    /// \brief Return the slave nodal total force
    /// \return Vector containing the slave nodal total force
    virtual inline const Vector3d* getTotalForceSlave() const { return &(this->totalForceSlaveBody); }
   
    /// brief Integrate momentum
    ///
    virtual void integrateMomentum(double dt);

    /// \brief Return the slave nodal mass
    /// \return slave Nodal mass
    virtual inline double getMassSlave() const { return massSlaveBody; }

    /// \brief Return the nodal unit normal
    /// \return nodal unit normal
    virtual inline const Vector3d* getNormal() const { return &(this->normal); }

    /// \brief Return the nodal unit normal slave
    /// \return nodal unit normal
    virtual inline const Vector3d* getNormalSlave() const { return &(this->normalSlave); }

    /// \brief Return the nodal unit normal total
    /// \return nodal unit normal
    virtual inline const Vector3d* getUnitNormalTotal() const { return &(this->unitNormalTotal); }

    /// \brief Return the nodal contact force
    /// \return nodal contact force
    virtual inline const Vector3d* getContactForce() const { return &(this->contactForce); }

    /// \brief Return the nodal normal contact force
    /// \return nodal contact force
    virtual inline const Vector3d* getNormalContactForce() const { return &(this->normalContactForce); }

    /// \brief Return the nodal tangential contact force
    /// \return nodal contact force
    virtual inline const Vector3d* getTangentialContactForce() const { return &(this->tangentialContactForce); }

    /// \brief Return the distance to the closest particle
    /// \return  distance to the closest particle
    virtual inline double getClosestParticleDistance() const { return this->closestParticleDistance; }

    /// \brief Return the distance to the closest particle slave
    /// \return  distance to the closest particle slave
    virtual inline double getClosestParticleDistanceSlave() const { return this->closestParticleDistanceSlave; }

    /// \brief Update nodal velocity of mixture
    ///
    virtual void updateVelocity();

    /// \brief Return the slave nodal velocity
    /// \return Vector containing the slave nodal velocity
    virtual inline const Vector3d* getVelocitySlave() const { return &(this->velocitySlaveBody); }

    /// \brief Configure the has contact flag
    /// Has contact flag
    virtual inline void setContactStatus(bool flag) { this->hasContact = flag; }

    /// \brief Return the contact status
    /// \return contact status
    virtual inline const bool getContactStatus() const { return (this->hasContact); }

    /// \brief Configure the has contact flag
    /// Has contact flag
    virtual inline void setSecondContactStatus(bool flag) { this->hasSecondContact = flag; }

    /// \brief Return the contact status
    /// \return contact status
    virtual inline const bool getSecondContactStatus() const { return (this->hasSecondContact); }

    /// \brief Set contact bodies Id
    /// contactBodyId
    /// contactBodySlaveId
    virtual inline void setContactBodies(int _contactBodyId, int _contactBodySlaveId) { this->contactBodyId = _contactBodyId, this->contactBodySlaveId = _contactBodySlaveId; }

    /// \brief Set the nodal unit normal total
    /// \set nodal unit normal
    virtual inline void setUnitNormalTotal(const Vector3d& _unitNormalTotal) { this->unitNormalTotal = _unitNormalTotal; }

    /// \brief Set the nodal unit normal
    /// \set nodal unit normal
    virtual inline void setNormal(const Vector3d& _normal) { this->normal = _normal; }

    /// \brief Set the nodal unit normal slave
    /// \set nodal unit normal
    virtual inline void setNormalSlave(const Vector3d& _normalSlave) { this->normalSlave = _normalSlave; }

    /// \brief Set the nodal contact force
    /// \set nodal contact force
    virtual inline void setContactForce(const Vector3d& _contactForce) { this->contactForce = _contactForce; }

    /// \brief Set the nodal normal contact force
    /// \set nodal contact force
    virtual inline void setNormalContactForce(const Vector3d& _normalContactForce) { this->normalContactForce = _normalContactForce; }

    /// \brief Set the tangential nodal contact force
    /// \set nodal contact force
    virtual inline void setTangentialContactForce(const Vector3d& _tangentialContactForce) { this->tangentialContactForce = _tangentialContactForce; }

    /// \brief Set the distance to the closest particle
    /// \set distance to the closest particle
    virtual inline void setClosestParticleDistance(double _closestParticleDistance) { this->closestParticleDistance = _closestParticleDistance; }

    /// \brief Set the distance to the closest particle slave
    /// \set distance to the closest particle slave
    virtual inline void setClosestParticleDistanceSlave(double _closestParticleDistanceSlave) { this->closestParticleDistanceSlave = _closestParticleDistanceSlave; }

    /// \brief Get contact bodies Id
    /// contactBodyId
    /// contactBodySlaveId
    virtual inline const int getContactBodyId(int pos) const {return  (pos == 0) ? this->contactBodyId : this->contactBodySlaveId; }

    int PA;
    int PB;

private:

    double massSlaveBody; //!< nodal mass: \f$m_{I}\f$, or solid mass in two-phase calculations: \f$m_{I}^{s}\f$

    Vector3d momentumSlaveBody; //!< nodal momentum: \f$p_{iI}\f$, or momentum in solid in two-phase calculations: \f$p_{iI}^{s}\f$
    Vector3d velocitySlaveBody; //!< nodal velocity: \f$v_{iI}\f$, or velocity in solid in two-phase calculations: \f$v_{iI}^{s}\f$
    Vector3d externalForceSlaveBody; //!< nodal external force: \f$f_{iI}^{ext}\f$, or external force in solid in two-phase calculations: \f$f_{iI}^{ext,s}\f$
    Vector3d internalForceSlaveBody; //!< nodal internal force: \f$f_{iI}^{int}\f$, or internal force in solid in two-phase calculations: \f$f_{iI}^{int,s}\f$
    Vector3d dampingForceSlaveBody; //!< nodal damping force: \f$f_{iI}^{dmp}\f$, or damping force in solid in two-phase calculations: \f$f_{iI}^{dmp,s}\f$
    Vector3d totalForceSlaveBody; //!< nodal total force: \f$f_{iI}\f$, or total force in solid in two-phase calculations: \f$f_{iI}^{s}\f$
    bool hasContact; //!< contact flag
    bool hasSecondContact; //!< contact flag
    int contactBodyId; //!< Body in contact at this node
    int contactBodySlaveId; //!< Slave body in contact at this node
    Vector3d normal;
    Vector3d normalSlave;
    Vector3d unitNormalTotal;
    Vector3d contactForce;
    Vector3d normalContactForce;
    Vector3d tangentialContactForce;
    double closestParticleDistance;
    double closestParticleDistanceSlave;
 
};

inline NodeContact::NodeContact()
{
    massSlaveBody = 0.0;
    closestParticleDistance = 999.0;
    closestParticleDistanceSlave = 999.0;
    momentumSlaveBody.setZero();
    velocitySlaveBody.setZero();
    externalForceSlaveBody.setZero();
    internalForceSlaveBody.setZero();   
    dampingForceSlaveBody.setZero();
    totalForceSlaveBody.setZero();
    normal.setZero();
    normalSlave.setZero();
    hasContact = false;
    hasSecondContact = false;
    contactBodyId = -1;
    contactBodySlaveId = -1;
    contactForce.setZero();
    unitNormalTotal.setZero();
}

inline void NodeContact::resetValues()
{
    Node::resetValues();

    massSlaveBody = 0.0;
    closestParticleDistance = 999.0;
    closestParticleDistanceSlave = 999.0;
    momentumSlaveBody.setZero();
    velocitySlaveBody.setZero();
    externalForceSlaveBody.setZero();
    internalForceSlaveBody.setZero();
    dampingForceSlaveBody.setZero();
    normal.setZero();
    normalSlave.setZero();
    hasContact = false;
    hasSecondContact = false;
    contactBodyId = -1;
    contactBodySlaveId = -1;
    contactForce.setZero();
    unitNormalTotal.setZero();
}

#endif /* INC_MESH_NODECONTACT_H_ */