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

void ParticleMixture::updateDensity() {

    // update density of solid
    Particle::updateDensity();

    // only saturated particle can be update
    if (this->getSaturation()<=0.0) { return; }

    // volumetric strain increment
    double volStrainInc = strainIncrementFluid.trace();
    
    // update particle density
    if ((1.0+volStrainInc)!=0.0){

        densityFluid = densityFluid / (1.0+volStrainInc);
    }
}

void ParticleMixture::updatePorosity() {

    // only saturated particle can be update
    if (this->getSaturation()<=0.0) { return; }

    double volumeSolid = Particle::getMass()/Particle::getDensity();
    double volumeFluid = this->getMassFluid()/this->getDensityFluid();
    
    this->porosityMixture = volumeFluid/(volumeFluid+volumeSolid);
}

void ParticleMixture::updatePressure(double dt) {

    // only saturated particle can be update
    if (this->getSaturation()<=0.0) { return; }

    // volumetric modulus of fluid
    double kw = material->getBulkModulusFluid();

    // porosity of mixture
    double n = this->porosityMixture;

    // volumetric strain rate of solid
    double vs = Particle::getStrainIncrement().trace()/dt;

    // volumetric strain rate of fluid
    double vf = (*(this->getStrainIncrementFluid())).trace()/dt;

    // pressure increment
    double dp = - dt * kw / n * ( (1 - n)*vs + n*vf );

    // update pressure
    this->pressureFluid+=dp;
}