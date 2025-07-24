// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef NEWTONIAN_H_
#define NEWTONIAN_H_

#include "Materials/Material.h"
#include "Particle/Particle.h"

/// \class Newtonian
/// \brief Represents a Newtonian fluid material model.
class Newtonian: public Material {

public:

    /// \brief Constructor for Newtonian fluid
    /// \param[in] id        Material ID
    /// \param[in] density   Fluid density \f$\rho\f$
    /// \param[in] viscosity Dynamic viscosity \f$\mu\f$
    /// \param[in] bulk      Bulk modulus \f$K\f$
    Newtonian(int id, double density, double viscosity, double bulk);

    /// \brief Default destructor
    virtual ~Newtonian();

    /// \brief Update stress of a particle
    /// \param[in] particle Pointer to the particle
    virtual void updateStress(Particle* particle) const override;

    /// \brief Get sound speed in the material
    /// \return sound speed \f$c = \sqrt{K / \rho}\f$
    virtual double getSoundSpeed() const override;

    /// \brief Return the material type
    inline virtual Material::MaterialType getType() const { return Material::getType(); }

protected:
    double Viscosity; //!< Dynamic viscosity \f$\mu\f$
    double Bulk;      //!< Bulk modulus \f$K\f$
};

inline Newtonian::Newtonian(int id, double density, double viscosity, double bulk)
: Material(id, density, Material::MaterialType::NEWTONIAN)
{
    this->Viscosity = viscosity;
    this->Bulk = bulk;
}

inline Newtonian::~Newtonian() { }

#endif /* NEWTONIAN_H_ */
