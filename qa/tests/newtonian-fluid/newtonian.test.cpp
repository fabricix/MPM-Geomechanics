#include <gtest/gtest.h>
#include "Material.h"
#include "Particle.h"
#include "Model.h"
#include "Newtonian.h"

#include <gtest/gtest.h>
#include "Material.h"
#include "Particle.h"
#include "Model.h"
#include "Newtonian.h"

TEST(MaterialNewtonian, PureShear) {
    
    // Set time step
    ModelSetup::setTimeStep(0.1);

    // Material properties
    double density = 1000.0;   // kg/m3
    double viscosity = 0.01;   // Pa·s
    double bulk = 1000.0;      // Pa

    // Create a Newtonian material
    Newtonian* newtonian = new Newtonian (1, density, viscosity, bulk);

    // Create a particle
    Particle particle(Vector3d(0.0, 0.0, 0.0), newtonian, Vector3d(1.0, 1.0, 1.0));
    
    // Set initial stress
    particle.setStress(Matrix3d::Zero());
    
    // Set time step for the model
    ModelSetup::setTimeStep(0.1);
    double dt = ModelSetup::getTimeStep();
    
    // strain increment
    double strain = 0.01;
    Matrix3d dE = Matrix3d::Identity() * strain;
    particle.setStrainIncrement(dE);

    // Update stress
    particle.updateStress( );
    Matrix3d stress = particle.getStress();
    
    // Expected stress is the hydrostatic pressure increment
    double expected_mean_stress = bulk * dE.trace();
    
    // Check if the stress is correctly updated
    EXPECT_NEAR(stress(0,0), expected_mean_stress, 1e-6) << "Failed at (0,0) in volumetric deformation case";
    EXPECT_NEAR(stress(1,1), expected_mean_stress, 1e-6) << "Failed at (1,1) in volumetric deformation case";
    EXPECT_NEAR(stress(2,2), expected_mean_stress, 1e-6) << "Failed at (2,2) in volumetric deformation case";
}

TEST(MaterialNewtonian, PureVolumetric) {
    
    // Set time step
    ModelSetup::setTimeStep(0.1);

    // Material properties
    double density = 1000.0;   // kg/m3
    double viscosity = 0.01;   // Pa·s
    double bulk = 1000.0;      // Pa

    // Create a Newtonian material
    Newtonian* newtonian = new Newtonian (1, density, viscosity, bulk);

    // Create a particle
    Particle particle(Vector3d(0.0, 0.0, 0.0), newtonian, Vector3d(1.0, 1.0, 1.0));
    
    // Set initial stress
    particle.setStress(Matrix3d::Zero());
    
    // Set time step for the model
    ModelSetup::setTimeStep(0.1);
    double dt = ModelSetup::getTimeStep();
    
    // Reset particle stress and strain increment    
    Matrix3d dE = Matrix3d::Zero();
    particle.setStress(Matrix3d::Zero());
    
    // Set pure shear strain increment in XY plane
    double strain = 0.02;
    dE(0,1) = strain; // Shear strain XY
    dE(1,0) = strain; // Shear strain YX
    particle.setStrainIncrement(dE);

    // Update stress (integrate stress increment)
    particle.updateStress( );
    Matrix3d stress = particle.getStress();
    
    // Expected pure shear
    double expected_shear_stress = 2 * viscosity * strain / dt;

    // Check if the stress is correctly updated
    EXPECT_NEAR(stress(0,1), expected_shear_stress, 1e-6) << "Failed at (0,1) in pure shear case";
    EXPECT_NEAR(stress(1,0), expected_shear_stress, 1e-6) << "Failed at (1,0) in pure shear case";
    EXPECT_NEAR(stress(0,0), 0.0, 1e-6) << "Failed at (0,0) in pure shear case";
    EXPECT_NEAR(stress(1,1), 0.0, 1e-6) << "Failed at (1,1) in pure shear case";
    EXPECT_NEAR(stress(2,2), 0.0, 1e-6) << "Failed at (2,2) in pure shear case";
}
