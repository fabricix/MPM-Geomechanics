#include <gtest/gtest.h>

#include "ElasticJaumann.h"
#include "MohrCoulomb.h"
#include "Particle.h"

TEST(ElasticJaumann, Rotates_Deviatoric_Stress_Without_Changing_Its_Trace)
{
  ElasticJaumann material(1, 1000.0, 1000.0, 0.25);
  Particle particle(Vector3d::Zero(), &material, Vector3d::Ones());
  Matrix3d initialStress = Matrix3d::Zero();
  initialStress.diagonal() << 2.0, -1.0, -1.0;
  particle.setStress(initialStress);

  Matrix3d spin = Matrix3d::Zero();
  spin(0, 1) = -0.1;
  spin(1, 0) = 0.1;
  particle.setVorticityIncrement(spin);
  particle.setStrainIncrement(Matrix3d::Zero());
  particle.updateStress();

  EXPECT_NEAR(particle.getStress()(0, 1), 0.3, 1e-12);
  EXPECT_NEAR(particle.getStress()(1, 0), 0.3, 1e-12);
  EXPECT_NEAR(particle.getStress().trace(), initialStress.trace(), 1e-12);
}

TEST(MohrCoulomb, Corrects_Shear_Yield_And_Applies_Dilation)
{
  MohrCoulomb::Softening noSoftening;
  MohrCoulomb zeroDilation(1, 1000.0, 1000.0, 0.25, 30.0, 0.1, 0.0, 100.0, noSoftening);
  MohrCoulomb dilation(2, 1000.0, 1000.0, 0.25, 30.0, 0.1, 20.0, 100.0, noSoftening);
  Particle particleZero(Vector3d::Zero(), &zeroDilation, Vector3d::Ones());
  Particle particleDilated(Vector3d::Zero(), &dilation, Vector3d::Ones());
  const Matrix3d strain = 0.01 * Matrix3d::Identity();
  particleZero.setStrainIncrement(strain);
  particleDilated.setStrainIncrement(strain);

  particleZero.updateStress();
  particleDilated.updateStress();

  EXPECT_GT(particleZero.getPlasticStrain(), 0.0);
  EXPECT_GT(particleDilated.getPlasticStrain(), 0.0);
  EXPECT_FALSE(particleZero.getStress().isApprox(particleDilated.getStress(), 1e-8));
}

TEST(MohrCoulomb, Softening_Reduces_Cohesion_And_Yield_Strength)
{
  MohrCoulomb::Softening softening;
  softening.softening_type = MohrCoulomb::Softening::EXPONENTIAL;
  softening.exponential_shape_factor = 1.0;
  softening.cohesion_residual = 0.01;
  softening.cohesion_softening_active = true;
  MohrCoulomb peakStrength(1, 1000.0, 1000.0, 0.25, 30.0, 0.1, 5.0, 100.0, MohrCoulomb::Softening{});
  MohrCoulomb softenedStrength(2, 1000.0, 1000.0, 0.25, 30.0, 0.1, 5.0, 100.0, softening);
  Particle peakParticle(Vector3d::Zero(), &peakStrength, Vector3d::Ones());
  Particle softenedParticle(Vector3d::Zero(), &softenedStrength, Vector3d::Ones());
  peakParticle.addPlasticStrain(1.0);
  softenedParticle.addPlasticStrain(1.0);
  peakParticle.setStrainIncrement(0.01 * Matrix3d::Identity());
  softenedParticle.setStrainIncrement(0.01 * Matrix3d::Identity());

  peakParticle.updateStress();
  softenedParticle.updateStress();

  EXPECT_NEAR(softening.exponentialSoftening(1.0, 1.0, 0.1, 0.01),
              0.01 + 0.09 * std::exp(-1.0), 1e-12);
  EXPECT_GT(peakParticle.getPlasticStrain(), 0.0);
  EXPECT_GT(softenedParticle.getPlasticStrain(), 0.0);
  EXPECT_FALSE(peakParticle.getStress().isApprox(softenedParticle.getStress(), 1e-8));
}
