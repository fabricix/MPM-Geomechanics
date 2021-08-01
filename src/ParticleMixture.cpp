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
    // local variables
    
    double porosity = material!=0?material->getPorosity():0.0;

    // configure members

    this->densityFluid = material!=0?material->getDensityFluid():0.0;

    this->massFluid = this->densityFluid*porosity*(size.x()*size.y()*size.z());
    
    this->pressureFluid=0.0;

    this->porosityMixture = porosity;
    
    this->velocityFluid.setZero();
    
    this->externalForceFluid.setZero();

    this->strainIncrementFluid.setZero();
}

ParticleMixture::~ParticleMixture() { }

Vector3d ParticleMixture::getDragForceFluid() const {

    double mgn = this->massFluid*(ModelSetup::getGravity()).norm()*this->porosityMixture;

    Vector3d drag;
    drag.x()=-mgn/this->material->getHydraulicConductivity().x()*(this->velocityFluid.x()-Particle::velocity.x());
    drag.y()=-mgn/this->material->getHydraulicConductivity().y()*(this->velocityFluid.y()-Particle::velocity.y());
    drag.z()=-mgn/this->material->getHydraulicConductivity().z()*(this->velocityFluid.z()-Particle::velocity.z());

    return drag;
}