// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INC_PARTICLE_PARTICLEMIXTURE_H_
#define INC_PARTICLE_PARTICLEMIXTURE_H_

#include <Particle/Particle.h>

/// \class ParticleMixture
/// \brief A material point for calculations using the mixture theory
class ParticleMixture: public Particle {

public:

    /// \brief Create a mixture particle
    /// \param[in] position Particle position
    /// \param[in] material Material
    /// \param[in] size Particle size in each direction
	ParticleMixture(const Vector3d& position, Material* material, const Vector3d& size);

    /// \brief Default virtual destructor
    virtual ~ParticleMixture();

    /// \brief Update the porosity of the skeleton
    ///
    virtual void updatePorosity();

    /// \brief Returns the mass of the fluid
    /// \return Fluid mass
    virtual inline double getMassFluid() const { return this->massFluid; }

    /// \brief Returns velocity of the fluid
    /// \return Current fluid velocity
    virtual inline const Vector3d* getVelocityFluid() const { return &(this->velocityFluid); }

    /// \brief Returns the external force of fluid
    /// \return Particle external force of fluid
    virtual inline const Vector3d* getExternalForceFluid() const { return &(this->externalForceFluid); }

    /// \brief Returns the drag force of fluid in skeleton
    /// \return Particle drag force of fluid
    virtual Vector3d getDragForceFluid() const;

    /// \brief Returns pressure of fluid
    /// \return Current pore pressure of fluid
    virtual inline double getPressureFluid() const { return this->pressureFluid; }

    /// \brief Returns current porosity
    /// \return Current porosity of mixture
    virtual inline double getPorosity() const { return this->porosityMixture; }

    /// \brief Returns initial porosity
    /// \return initial porosity of mixture
    virtual inline double getInitialPorosity() const { return this->initialPorosityMixture; }

    /// \brief Returns the current particle density of fluid
    /// \return Particle density of fluid
    virtual inline double getDensityFluid() const { return this->densityFluid; }

    /// \brief Adds a external fluid force increment
    /// \param[in] delta_external_fluid_force External fluid force increment
    virtual inline void addExternalForceFluid(const Vector3d& delta_external_fluid_force) { this->externalForceFluid+=delta_external_fluid_force; }

    /// \brief Configures particle velocity of fluid phase
    /// \param[in] particle_velocity_fluid Current particle velocity of fluid phase
    virtual inline void setVelocityFluid(const Vector3d& particle_velocity_fluid) { this->velocityFluid=particle_velocity_fluid;}

    /// \brief Configures the strain increment of fluid phase
    /// \param[in] strain_increment_fluid Particle strain increment of fluid phase
    virtual inline void setStrainIncrementFluid(const Matrix3d& strain_increment_fluid) { this->strainIncrementFluid=strain_increment_fluid; }

    /// \brief Configures the material in the particle
    /// \param[in] material Material
    virtual void setMaterial(Material* material);
    
    /// \brief Returns the strain increment of fluid
    /// \return Particle strain increment of fluid
    virtual inline const Matrix3d* getStrainIncrementFluid() const { return &(this->strainIncrementFluid); }

    /// \brief Update the particle pressure
    ///  
    virtual void updatePressure(double dt);

    /// \brief Returns Saturation of fluid in void in mixture
    /// \return saturation
    virtual inline double getSaturation() const { return 1.0; }

    /// \brief Configure the pressure of fluid
    /// \param[in] pressure Pressure of the fluid
    virtual inline void setPressureFluid(double pressure) { this->pressureFluid=pressure; }

    /// \brief Returns current particle volume
    /// \return Particle volume
    virtual double getCurrentVolume() const;

private:

    double massFluid; //!< fluid mass in mixture: \f$m^{f}\f$
    double densityFluid; //!< current fluid density in mixture: \f$\rho^{f}\f$
    double pressureFluid; //!< current fluid pressure in mixture: \f$p^{f}\f$
    double porosityMixture; //!< current porosity of the mixture: \f$n\f$
    double initialPorosityMixture; //!< initial porosity of the mixture: \f$n^{0}\f$
    
    Vector3d velocityFluid; //!< current fluid velocity in mixture: \f$v_i^{f}\f$
    Vector3d externalForceFluid; //!< current external force of fluid in mixture: \f$f_i^{ext,f}\f$
    Vector3d internalForceFluid; //!< current internal force of fluid in mixture: \f$f_i^{int,f}\f$
    Matrix3d strainIncrementFluid; //!< current fluid strain increment in mixture: \f$\Delta \epsilon_{ij}^{f}\f$
};

#endif /* INC_PARTICLE_PARTICLEMIXTURE_H_ */