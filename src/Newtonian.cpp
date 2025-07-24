// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <cmath>
#include "Materials/Newtonian.h"
#include "Particle/Particle.h"
#include "Model.h"

// ------------------------------------------------------------------------
// Newtonian fluid stress update
// ------------------------------------------------------------------------
// For a Newtonian fluid:
//   σ = -p I + 2 μ D
//
// where D is the symmetric strain rate tensor:
//   D_ij = 0.5 * (∂v_i/∂x_j + ∂v_j/∂x_i)
//
// In this implementation:
// - The diagonal terms dE(xx), dE(yy), dE(zz) are ∂v_x/∂x, etc., 
//   so we multiply them by 2μ.
//
// - The off-diagonal terms dE(xy), dE(xz), dE(yz) already include the 0.5
//   factor from the symmetric definition of D. Thus, we use 2μ * dE_ij 
//   to correctly apply 2 μ D.
// -------------------------------------------------------------------------

void Newtonian::updateStress(Particle* particle) const {

    // Get strain increment from the particle
    // dE represents the increment of strain over the time step dt
    Matrix3d dE = particle->getStrainIncrement();
    
    double dt = ModelSetup::getTimeStep();

    // Compute strain rate tensor: D = dE / dt
    Matrix3d D = dE / dt;

    // Volumetric strain rate: trace(D)
    double volumetric_strain_rate = D.trace();

    // Update pressure: p = p_prev + K * (trace(D) * dt)
    double pressure_increment = Bulk * volumetric_strain_rate * dt;
    double pressure = particle->getStress().trace() / 3.0 + pressure_increment;

    // Total stress tensor (Cauchy stress)
    // σ = -p I + 2 μ D
    Matrix3d stress = -pressure * Matrix3d::Identity() + 2.0 * Viscosity * D;

    // Set the updated stress in the particle
    particle->setStress(stress);
}

double Newtonian::getSoundSpeed() const {
    // For fluids, sound speed = sqrt(K / density)
    return std::sqrt(Bulk / density);
}
