/*
 * NodeMixture.h
 *
 *  Created on: 23 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INC_MESH_NODEMIXTURE_H_
#define INC_MESH_NODEMIXTURE_H_

#include <Mesh/Node.h>

/// \class NodeMixture
/// \brief Represents a mesh node for calculations using the mixture theory
class NodeMixture: public Node {

public:

    /// \brief Default constructor
    ///
	NodeMixture();

    /// \brief Default destructor
    ///
	virtual ~NodeMixture();

    /// \brief Delete all values stored in node
    ///
    virtual void resetValues();

    /// \brief Add fluid mass increment to the nodal mass
    /// \param[in] fluid_mass_increment Nodal mass increment 
    virtual inline void addMassFluid(double fluid_mass_increment) { this->massFluid+=fluid_mass_increment; }

    /// \brief Add fluid momentum increment to the nodal momentum of fluid
    /// \param[in] fluid_momentum_increment Vector containing the nodal momentum increment of fluid
    virtual inline void addMomentumFluid(const Vector3d& fluid_momentum_increment) { this->momentumFluid+=fluid_momentum_increment; }

    /// \brief Add a internal force increment of fluid to the nodal internal force
    /// \param[in] internal_force_fluid_increment Vector containing nodal internal force increment of fluid
    virtual inline void addInternalForceFluid(const Vector3d& internal_force_fluid_increment) { this->internalForceFluid+=internal_force_fluid_increment; }

    /// \brief Add a external force of fluid increment to the nodal external force
    /// \param[in] external_force_fluid_increment Vector containing nodal external force increment 
    virtual inline void addExternalForceFluid(const Vector3d& external_force_fluid_increment) { this->externalForceFluid+=external_force_fluid_increment; }

    /// \brief Configure the nodal momentum of fluid phase
    /// \param[in] nodal_momentum_fluid Vector containing the nodal momentum of fluid phase
    virtual inline void setMomentumFluid(const Vector3d& nodal_momentum_fluid) { this->momentumFluid=nodal_momentum_fluid; }

    /// \brief Return the nodal momentum of fluid phase
    /// \return Vector containing the nodal momentum of fluid phase
    virtual inline const Vector3d* getMomentumFluid() const { return &(this->momentumFluid); }

    /// \brief Calculate the total nodal force
    ///
    virtual void updateTotalForce();

    /// \brief Calculate the damping nodal force
    ///
    virtual void updateDampingForce();

    /// \brief Configure the total nodal force in fluid phase
    /// \param[in] total_nodal_force_fluid Vector containing the total
    /// nodal force in fluid phase
    virtual inline void setTotalForceFluid(const Vector3d& total_nodal_force_fluid) { this->totalForceFluid=total_nodal_force_fluid; }

    /// \brief Return the nodal total force of fluid phase
    /// \return Vector containing the nodal total force of fluid phase
    virtual inline const Vector3d* getTotalForceFluid() const { return &(this->totalForceFluid); }

    /// brief Integrate momentum
    ///
    virtual void integrateMomentum(double dt);

    /// \brief Return the nodal mass of fluid phase
    /// \return Nodal mass of fluid phase
    virtual inline double getMassFluid() const { return massFluid; }

    /// \brief Update nodal velocity of mixture
    ///
    virtual void updateVelocity();

    /// \brief Return the nodal velocity of fluid phase
    /// \return Vector containing the nodal velocity of fluid phase
    virtual inline const Vector3d* getVelocityFluid() const { return &(this->velocityFluid); }

private:

    double massFluid; //!< nodal mass of fluid: \f$m_I^{f}\f$

    Vector3d momentumFluid; //!< nodal momentum of fluid: \f$p_{iI}^{f}\f$
    Vector3d velocityFluid; //!< nodal velocity of fluid: \f$v_{iI}^{f}\f$
    Vector3d externalForceFluid; //!< nodal external force of fluid: \f$f_{iI}^{\text{ext},f}\f$
    Vector3d internalForceFluid; //!< nodal internal force of fluid: \f$f_{iI}^{\text{int},f}\f$
    Vector3d dampingForceFluid; //!< nodal damping force of fluid: \f$f_{iI}^{\text{dmp},f}\f$
    Vector3d totalForceFluid; //!< nodal total force of fluid: \f$f_{iI}^{f}\f$
};

inline NodeMixture::NodeMixture()
{
    massFluid=0;
    momentumFluid.setZero();
    velocityFluid.setZero();
    externalForceFluid.setZero();
    internalForceFluid.setZero();
    dampingForceFluid.setZero();
    totalForceFluid.setZero();
}

inline void NodeMixture::resetValues()
{
    Node::resetValues();

    massFluid=0.0;
    momentumFluid.setZero();
    externalForceFluid.setZero();
    internalForceFluid.setZero();
}

#endif /* INC_MESH_NODEMIXTURE_H_ */