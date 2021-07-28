/*
 * ParticleMixture.cpp
 *
 *  Created on: 22 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <Particle/ParticleMixture.h>


ParticleMixture::ParticleMixture(const Vector3d& position, Material* material, const Vector3d& size):
Particle(position, material, size)
{
    // TODO

    // this->massFluid=0; //!< fluid mass
    
    // this->densityFluid=0; //!< particle density

    // this->pressure=0; //!< fluid pressure

    // this->initialPorosity=0; //!< initial porosity of the medium

    // this->porosity=0; //!< porosity of the medium
    
    // Vector3d velocityFluid; //!< fluid velocity
    
    // Vector3d externalForceFluid; //!< external force of fluid

    // Matrix3d strainIncrementFluid; //!< fluid strain increment

}

ParticleMixture::~ParticleMixture() { }