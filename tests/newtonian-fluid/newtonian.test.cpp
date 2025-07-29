#include <gtest/gtest.h>
#include "Material.h"
#include "Particle.h"
#include "ModelSetup.h"
#include "Newtonian.h"

TEST(MaterialNewtonian, UpdateStress_SimpleCases) {
    // Set time step
    ModelSetup::setTimeStep(0.1); // 0.1 seconds

    // Material properties
    double density = 1000.0;   // kg/m3
    double viscosity = 0.01;   // Pa·s
    double bulk = 1000.0;      // Pa

    // Create a Newtonian material
    Newtonian newtonian(0, density, viscosity, bulk);

    // Create a particle
    Particle particle(Vector3d(0.0, 0.0, 0.0), nullptr, Vector3d(1.0, 1.0, 1.0));
    particle.setStress(Matrix3d::Zero());  // Initial stress = 0

    // ----------------------------------------------------------------------
    // Case 1: Pure volumetric deformation
    // ----------------------------------------------------------------------
    Matrix3d dE_vol = Matrix3d::Identity() * 0.01;  // 1% strain in all directions
    particle.setStrainIncrement(dE_vol);

    newtonian.updateStress(&particle);
    Matrix3d stress_vol = particle.getStress();

    // Expected stress: only diagonal terms (hydrostatic pressure)
    double expected_mean_stress = bulk * (0.01 + 0.01 + 0.01);
    EXPECT_NEAR(stress_vol(0,0), expected_mean_stress, 1e-6);
    EXPECT_NEAR(stress_vol(1,1), expected_mean_stress, 1e-6);
    EXPECT_NEAR(stress_vol(2,2), expected_mean_stress, 1e-6);
    EXPECT_NEAR(stress_vol(0,1), 0.0, 1e-6);

    // ----------------------------------------------------------------------
    // Case 2: Pure shear
    // ----------------------------------------------------------------------
    Matrix3d dE_shear = Matrix3d::Zero();
    dE_shear(0,1) = 0.02; // Shear strain XY
    dE_shear(1,0) = 0.02;
    particle.setStress(Matrix3d::Zero());
    particle.setStrainIncrement(dE_shear);

    newtonian.updateStress(&particle);
    Matrix3d stress_shear = particle.getStress();

    // For pure shear, sigma_xy = mu * gamma_dot
    double gamma_dot = dE_shear(0,1) / ModelSetup::getTimeStep();
    double expected_shear_stress = viscosity * gamma_dot;

    EXPECT_NEAR(stress_shear(0,1), expected_shear_stress, 1e-6);
    EXPECT_NEAR(stress_shear(1,0), expected_shear_stress, 1e-6);
    EXPECT_NEAR(stress_shear(0,0), 0.0, 1e-6);
    EXPECT_NEAR(stress_shear(1,1), 0.0, 1e-6);
}
