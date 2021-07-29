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

private:

    double massFluid; //!< fluid mass in mixture: \f$m^{f}\f$
    double densityFluid; //!< current fluid density in mixture: \f$\rho^{f}\f$
    double pressure; //!< current fluid pressure in mixture: \f$p^{f}\f$
    double porosity; //!< current porosity of the mixture: \f$n\f$
    
    Vector3d velocityFluid; //!< current fluid velocity in mixture: \f$v_i^{f}\f$
    Vector3d externalForceFluid; //!< current external force of fluid in mixture: \f$f_i^{\text{ext},f}\f$
    Vector3d internalForceFluid; //!< current internal force of fluid in mixture: \f$f_i^{\text{int},f}\f$
    Matrix3d strainIncrementFluid; //!< current fluid strain increment in mixture: \f$\Delta \epsilon_{ij}^{f}\f$
};

#endif /* INC_PARTICLE_PARTICLEMIXTURE_H_ */
