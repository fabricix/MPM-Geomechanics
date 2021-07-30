/*
 * ParticleMixture.h
 *
 *  Created on: 22 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

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

    /// \brief Returns the mass of the particle
    /// \return Particle mass
    virtual inline double getMassFluid() const { return this->massFluid; }

    /// \brief Returns velocity of fluid
    /// \return Current velocity of fluid
    virtual inline const Vector3d* getVelocityFluid() const { return &(this->velocityFluid); }

    /// \brief Returns the external force of fluid in particle
    /// \return Particle external force of fluid
    virtual inline const Vector3d* getExternalForceFluid() const { return &(this->externalForceFluid); }

    /// \brief Returns pressure of fluid
    /// \return Current pore pressure of fluid
    virtual inline double getPressureFluid() const { return this->pressureFluid; }

    /// \brief Returns current porosity
    /// \return Current porosity of mixture
    virtual inline double getPorosity() const { return this->porosityMixture; }

    /// \brief Returns the current particle density of fluid
    /// \return Particle density of fluid
    virtual inline double getDensityFluid() const { return this->densityFluid; }

    /// \brief Adds a external fluid force increment
    /// \param[in] delta_external_fluid_force External fluid force increment
    virtual inline void addExternalForceFluid(const Vector3d& delta_external_fluid_force) { this->externalForceFluid+=delta_external_fluid_force; }

private:

    double massFluid; //!< fluid mass in mixture: \f$m^{f}\f$
    double densityFluid; //!< current fluid density in mixture: \f$\rho^{f}\f$
    double pressureFluid; //!< current fluid pressure in mixture: \f$p^{f}\f$
    double porosityMixture; //!< current porosity of the mixture: \f$n\f$
    
    Vector3d velocityFluid; //!< current fluid velocity in mixture: \f$v_i^{f}\f$
    Vector3d externalForceFluid; //!< current external force of fluid in mixture: \f$f_i^{\text{ext},f}\f$
    Vector3d internalForceFluid; //!< current internal force of fluid in mixture: \f$f_i^{\text{int},f}\f$
    Matrix3d strainIncrementFluid; //!< current fluid strain increment in mixture: \f$\Delta \epsilon_{ij}^{f}\f$
};

#endif /* INC_PARTICLE_PARTICLEMIXTURE_H_ */
