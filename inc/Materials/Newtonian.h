// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef NEWTONIAN_H_
#define NEWTONIAN_H_

#include "Materials/Material.h"
#include "Particle/Particle.h"

/// \class Newtonian
/// \brief Represents a Newtonian fluid.
class Newtonian: public Material {

public:

    /// \brief Create a Newtonian fluid material
    /// \param[in] id Material identification
    /// \param[in] density Fluid density \f$\rho\f$
    /// \param[in] viscosity Dynamic viscosity \f$\mu\f$
    /// \param[in] bulk Bulk modulus \f$K\f$
    Newtonian(int id, double density, double viscosity, double bulk);
    
    /// \brief Default destructor
    virtual ~Newtonian();
    
    /// \brief Configure the dynamic viscosity
    /// \param[in] viscosity Dynamic viscosity \f$\mu\f$
    inline void setViscosity(double viscosity) { this->Viscosity = viscosity; }
    
    /// \brief Configure the bulk modulus
    /// \param[in] bulk Bulk modulus \f$K\f$
    inline void setBulk(double bulk) { this->Bulk = bulk; }

    /// \brief Update the stress in the particle
    /// \param[in] particle Particle
    virtual void updateStress(Particle* particle) const override;

    /// \brief Return the material type
    /// \return Material type Material::MaterialType
    inline virtual Material::MaterialType getType() const { return Material::getType(); }

    /// \brief Get sound speed
    /// \return sound_speed Sound speed
    virtual double getSoundSpeed() const override;

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
