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
	
    virtual ~ParticleMixture();

private:

    double massFluid; //!< fluid mass in mixture: \f$m^{f,t}\f$
    double densityFluid; //!< current fluid density in mixture: \f$\rho^{f,t}\f
    double pressure; //!< current fluid pressure in mixture: \f$p^{f,t}\f$
    double porosity; //!< current porosity of the mixture: \f$n^{t}\f$
    
    Vector3d velocityFluid; //!< current fluid velocity in mixture: \f$v_i^{f,t}\f$
    Vector3d externalForceFluid; //!< current external force of fluid in mixture: \f$f_i^{\text{ext},f,t}\f$
    Vector3d internalForceFluid; //!< current internal force of fluid in mixture: \f$f_i^{\text{int},f,t}\f$
    Matrix3d strainIncrementFluid; //!< current fluid strain increment in mixture: \f$\Delta \epsilon_{ij}^{f,t}\f$
};

#endif /* INC_PARTICLE_PARTICLEMIXTURE_H_ */
