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