/*
 * ParticleMixture.cpp
 *
 *  Created on: 22 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <Particle/ParticleMixture.h>
#include <Eigen/Dense>

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

    this->initialPorosityMixture = porosity;
    
    this->velocityFluid.setZero();
    
    this->externalForceFluid.setZero();

    this->strainIncrementFluid.setZero();
}

void ParticleMixture::setMaterial(Material* material) { 

    // update quantities in solid particle
    Particle::setMaterial(material);

    // update attributes of the mixture
    double porosity = material!=0?material->getPorosity():0.0;
    this->porosityMixture = porosity;
    this->densityFluid = material!=0?material->getDensityFluid():0.0;
    this->massFluid = this->densityFluid*porosity*(size.x()*size.y()*size.z());
}

ParticleMixture::~ParticleMixture() { }

Vector3d ParticleMixture::getDragForceFluid() const {

    Vector3d drag;
    drag.x()= -this->massFluid*9.81*this->porosityMixture/this->material->getHydraulicConductivity().x()*(this->velocityFluid.x()-Particle::velocity.x());
    drag.y()= -this->massFluid*9.81*this->porosityMixture/this->material->getHydraulicConductivity().y()*(this->velocityFluid.y()-Particle::velocity.y());
    drag.z()= -this->massFluid*9.81*this->porosityMixture/this->material->getHydraulicConductivity().z()*(this->velocityFluid.z()-Particle::velocity.z());

    return drag;
}

void ParticleMixture::updatePorosity() {

    // only saturated particle can be update
    if (this->getSaturation()<=0.0) { return; }

    // compute the Jacobian of the motion: J = V^{n+1}/V^0 = det (F)
    double J = this->deformationGradient.determinant();
    
    // update porosity
    if (J!=0){

        this->porosityMixture = 1.0 - ( 1.0 - this->getInitialPorosity() ) / J;
    }
}

void ParticleMixture::updatePressure(double dt) {

    // only saturated particle can be update
    if (this->getSaturation()<=0.0) { return; }

    // volumetric modulus of fluid
    double kw = material->getBulkModulusFluid();

    // porosity of mixture
    double n = this->porosityMixture;

    // volumetric strain rate of solid
    double grad_vs = Particle::getStrainIncrement().trace()/dt;

    // volumetric strain rate of fluid
    double grad_vf = (*(this->getStrainIncrementFluid())).trace()/dt;

    // pressure increment
    double dp = -dt*kw/n*((1-n)*grad_vs+n*grad_vf);

    // update pressure
    this->pressureFluid+=dp;
}

double ParticleMixture::getCurrentVolume() const {

    // mixture volume
    double totalMass = Particle::getMass() + ParticleMixture::getMassFluid();
    double porosity = ParticleMixture::getPorosity();
    double fluidDensity = ParticleMixture::getDensityFluid();
    double solidDensity = Particle::getDensity();
    double currentVolumeMixture = totalMass / (porosity * fluidDensity + (1 - porosity) * solidDensity);
    return currentVolumeMixture;
}