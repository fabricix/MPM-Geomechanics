// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <cmath>
#include "Materials/Newtonian.h"
#include "Particle/Particle.h"

void Newtonian::updateStress(Particle* particle) const {

    double dt = ModelSetup::getTimeStep();

    // strain rate tensor
    Matrix3d D = particle->getStrainIncrement() / dt;
    double Dvol = D.trace(); // volumetric strain rate
    Matrix3d Ddev = D - Matrix3d::Identity() * Dvol /3.0; // deviatoric strain rate tensor

    // mean stress update
    double mean_stress = particle->getStress().trace()/3.0 + Bulk * Dvol * dt;

    // Set updated stress in the particle
    particle->setStress(Matrix3d::Identity()*mean_stress + 2.0 * Viscosity * Ddev);
}

double Newtonian::getSoundSpeed() const {
    // Sound speed for fluids: c = sqrt(K / ρ)
    return std::sqrt(Bulk / density);
}