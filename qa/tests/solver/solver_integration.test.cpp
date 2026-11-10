#include <gtest/gtest.h>

#include <filesystem>
#include <vector>

#include "BodyParticle.h"
#include "Elastic.h"
#include "Model.h"
#include "Output.h"
#include "Particle.h"
#include "ParticleMixture.h"
#include "ShapeGimp.h"
#include "Solver/SolverExplicit.h"
#include "Solver/SolverExplicitTwoPhaseUSL.h"
#include "TestTemporaryDirectory.h"

namespace
{
class CurrentDirectoryGuard
{
public:
  explicit CurrentDirectoryGuard(const std::filesystem::path& directory)
      : previous_(std::filesystem::current_path())
  {
    std::filesystem::current_path(directory);
  }

  ~CurrentDirectoryGuard()
  {
    std::filesystem::current_path(previous_);
  }

private:
  std::filesystem::path previous_;
};

void configureOneStepModel(bool twoPhase)
{
  ModelSetup::setTime(0.001);
  ModelSetup::setTimeStep(0.001);
  ModelSetup::setResultNum(1);
  ModelSetup::setLoopCounter(0);
  ModelSetup::setCurrentTime(0.0);
  ModelSetup::setGravity(Vector3d::Zero());
  ModelSetup::setTwoPhaseActive(twoPhase);
  ModelSetup::setContactActive(false);
  ModelSetup::setTerrainContactActive(false);
  ModelSetup::setDampingType(ModelSetup::UNDAMPED);
  Output::configureResultFields({"none"});
  Output::configureGridResultFields({"none"});
  Output::configureSTLContactFields({"none"});
}

void releaseMesh(Mesh& mesh)
{
  for (auto* cell : *mesh.getCells())
    delete cell;
  for (auto* node : *mesh.getNodes())
    delete node;
  mesh.getCells()->clear();
  mesh.getNodes()->clear();
}
}

TEST(SolverIntegration, AdvancesOnePhaseAndTwoPhaseModelsWithoutExternalLoads)
{
  TestTemporaryDirectory directory;
  CurrentDirectoryGuard currentDirectory(directory.path());

  {
    configureOneStepModel(false);
    Mesh mesh;
    mesh.setNumCells(3, 3, 3);
    mesh.setCellDimension(1.0, 1.0, 1.0);
    mesh.createGrid(false);

    Elastic material(1, 2000.0, 100000.0, 0.25);
    ShapeGimp shape;
    Particle particle(Vector3d(1.5, 1.5, 1.5), &material, Vector3d(0.5, 0.5, 0.5));
    particle.setShape(&shape);
    const double initialMass = particle.getMass();
    BodyParticle body;
    body.setParticles({&particle});
    std::vector<Body*> bodies{&body};
    std::vector<Particle*> particles{&particle};

    SolverExplicit solver;
    solver.registerMesh(&mesh);
    solver.registerBodies(&bodies);
    solver.registerParticles(&particles);
    solver.Solve();

    EXPECT_TRUE(particle.getPosition().isApprox(Vector3d(1.5, 1.5, 1.5)));
    EXPECT_TRUE(particle.getVelocity().isApprox(Vector3d::Zero()));
    EXPECT_TRUE(particle.getStress().isApprox(Matrix3d::Zero()));
    EXPECT_DOUBLE_EQ(particle.getMass(), initialMass);
    releaseMesh(mesh);
  }

  {
    configureOneStepModel(true);
    Mesh mesh;
    mesh.setNumCells(3, 3, 3);
    mesh.setCellDimension(1.0, 1.0, 1.0);
    mesh.createGrid(true);

    Elastic material(2, 2000.0, 100000.0, 0.25);
    material.setPorosity(0.3);
    material.setDensityFluid(1000.0);
    material.setBulkModulusFluid(2.0e6);
    material.setHydraulicConductivity(Vector3d::Ones());
    ShapeGimp shape;
    ParticleMixture particle(Vector3d(1.5, 1.5, 1.5), &material, Vector3d(0.5, 0.5, 0.5));
    particle.setShape(&shape);
    const double initialSolidMass = particle.getMass();
    const double initialFluidMass = particle.getMassFluid();
    BodyParticle body;
    body.setParticles({&particle});
    std::vector<Body*> bodies{&body};
    std::vector<Particle*> particles{&particle};

    SolverExplicitTwoPhaseUSL solver;
    solver.registerMesh(&mesh);
    solver.registerBodies(&bodies);
    solver.registerParticles(&particles);
    solver.Solve();

    EXPECT_TRUE(particle.getPosition().isApprox(Vector3d(1.5, 1.5, 1.5)));
    EXPECT_TRUE(particle.getVelocity().isApprox(Vector3d::Zero()));
    EXPECT_TRUE(particle.getStress().isApprox(Matrix3d::Zero()));
    EXPECT_DOUBLE_EQ(particle.getMass(), initialSolidMass);
    EXPECT_DOUBLE_EQ(particle.getMassFluid(), initialFluidMass);
    releaseMesh(mesh);
  }

  ModelSetup::setTwoPhaseActive(false);
}
