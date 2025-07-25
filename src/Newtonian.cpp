// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <cmath>
#include "Materials/Newtonian.h"
#include "Particle/Particle.h"

void Newtonian::updateStress(Particle* particle) const {

    // strain rate increment
    Matrix3d dE = particle->getStrainIncrement();
    double dt = ModelSetup::getTimeStep();

    // Compute strain rate tensor: D = dE / dt
    Matrix3d D = dE / dt;
   
    // Volumetric strain rate (trace of D)
    double volumetric_strain_rate = D.trace();

    // pressure increment
    double pressure_increment = Bulk * volumetric_strain_rate * dt;

    // update pressure
    double pressure = particle->getStress().trace()/3.0 + pressure_increment;

    // volumetric part
    double stress_vol = pressure - 2.0 * Viscosity * volumetric_strain_rate/3.0;

    // update stress components
    
    Matrix3d stress = Matrix3d::Zero();

    // Diagonal components
    stress(0,0) = stress_vol + 2.0 * Viscosity * dE(0,0);
    stress(1,1) = stress_vol + 2.0 * Viscosity * dE(1,1);
    stress(2,2) = stress_vol + 2.0 * Viscosity * dE(2,2);

    // Off-diagonal components
    stress(0,1) = stress(1,0) = Viscosity * dE(0,1);
    stress(0,2) = stress(2,0) = Viscosity * dE(0,2);
    stress(1,2) = stress(2,1) = Viscosity * dE(1,2);

    // Set updated stress in the particle
    particle->setStress(stress);
}

double Newtonian::getSoundSpeed() const {
    // Sound speed for fluids: c = sqrt(K / ρ)
    return std::sqrt(Bulk / density);
}