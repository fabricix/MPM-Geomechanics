#include <gtest/gtest.h>

#include <fstream>
#include <vector>

#include "HydroMechanicalCoupling.h"
#include "Input.h"
#include "Model.h"
#include "Particle.h"
#include "TestTemporaryDirectory.h"

TEST(HydroMechanicalCoupling, Assigns_Pore_Pressure_To_Nearby_Particles)
{
  TestTemporaryDirectory directory;
  const auto pressureFile = directory.path() / "pressure.json";
  const auto configFile = directory.path() / "input.json";

  {
    std::ofstream pressure(pressureFile);
    ASSERT_TRUE(pressure.is_open());
    pressure << R"([{"position":[1.0,1.0,1.0],"pressure":125.0}])";
  }
  {
    std::ofstream config(configFile);
    ASSERT_TRUE(config.is_open());
    config << R"({"mesh":{"cells_dimension":[1.0,1.0,1.0]},"hydro_mechanical_coupling":{"enabled":true,"type":"one_way","pore_pressure_field":")"
           << pressureFile.generic_string() << R"("}})";
  }

  Input::readInputFile(configFile.string());
  Particle nearby(Vector3d(1.0, 1.0, 1.0), nullptr, Vector3d::Ones());
  Particle distant(Vector3d(4.0, 4.0, 4.0), nullptr, Vector3d::Ones());
  std::vector<Particle*> particles{&nearby, &distant};

  ModelSetup::setHydroMechanicalCouplingType(ModelSetup::HydroMechanicalCouplingType::NONE);
  HydroMechanicalCoupling::configureOneDirectionCoupling(particles);

  EXPECT_TRUE(HydroMechanicalCoupling::isOneWayEnabled());
  EXPECT_DOUBLE_EQ(nearby.getPorePressure(), 125.0);
  EXPECT_DOUBLE_EQ(distant.getPorePressure(), 0.0);
  ModelSetup::setHydroMechanicalCouplingType(ModelSetup::HydroMechanicalCouplingType::NONE);
}

TEST(HydroMechanicalCoupling, Computes_Total_Stress)
{
  const Matrix3d effectiveStress =
      (Matrix3d() << 2.0, 0.5, 0.0,
                     0.5, 3.0, 0.0,
                     0.0, 0.0, 4.0).finished();

  EXPECT_TRUE(HydroMechanicalCoupling::computeTotalStress(effectiveStress, 0.0)
                  .isApprox(effectiveStress));
  EXPECT_TRUE(HydroMechanicalCoupling::computeTotalStress(effectiveStress, 10.0)
                  .isApprox(effectiveStress + 10.0 * Matrix3d::Identity()));
}
