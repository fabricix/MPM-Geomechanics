#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Body/BodyGmsh.h"
#include "Eigen/Geometry"
#include "GmshMeshReader.h"
#include "Input.h"
#include "Model.h"
#include "Particle/ParticleMixture.h"

namespace {

namespace fs = std::filesystem;

fs::path unitCubeMesh() {
    return fs::absolute(fs::path(__FILE__)).parent_path().parent_path()
        .parent_path().parent_path() /
        "examples/bodies/createBodyGmsh/cuboid.msh";
}

struct ParticleTotals {
    std::size_t count = 0;
    std::size_t mixtureCount = 0;
    double mass = 0.0;
    double currentVolume = 0.0;
};

class BodyGmshTest : public ::testing::Test {
protected:
    void SetUp() override {
        previousTwoPhase_ = ModelSetup::getTwoPhaseActive();
        BodyGmsh::resetSharedMesh();
        ASSERT_TRUE(fs::is_regular_file(unitCubeMesh()));

        const auto stamp = std::chrono::steady_clock::now()
            .time_since_epoch().count();
        for (unsigned attempt = 0; attempt < 100; ++attempt) {
            const fs::path candidate = fs::temp_directory_path() /
                ("mpm-gmsh-test-" + std::to_string(stamp) + "-" +
                 std::to_string(attempt));
            if (fs::create_directory(candidate)) {
                temporaryDirectory_ = candidate;
                break;
            }
        }
        ASSERT_FALSE(temporaryDirectory_.empty());
    }

    void TearDown() override {
        clearBodiesAndMaterials();
        BodyGmsh::resetSharedMesh();
        ModelSetup::setTwoPhaseActive(previousTwoPhase_);
        if (!temporaryDirectory_.empty()) {
            std::error_code error;
            fs::remove_all(temporaryDirectory_, error);
        }
    }

    ParticleTotals createCube(const std::string& distribution,
                             double cellLength = 1.0,
                             bool twoPhase = false,
                             double porosity = 0.0) {
        clearBodiesAndMaterials();
        BodyGmsh::resetSharedMesh();
        ModelSetup::setTwoPhaseActive(twoPhase);

        json input = {
            {"material", json::array({{
                {"type", "elastic"}, {"id", 1},
                {"young", 1000000.0}, {"poisson", 0.3},
                {"density", 2000.0}, {"density_fluid", 1000.0},
                {"porosity", porosity}
            }})},
            {"body", json::array({{
                {"type", "gmsh"}, {"id", 1},
                {"mesh_file", unitCubeMesh().generic_string()},
                {"physical_to_material", {{"Cuboid", 1}}}
            }})}
        };
        if (!distribution.empty()) {
            input["body"][0]["particle_distribution"] = distribution;
        }

        const fs::path inputPath = temporaryDirectory_ / "input.json";
        {
            std::ofstream output(inputPath);
            if (!output || !(output << input.dump(2))) {
                ADD_FAILURE() << "Cannot write the temporary Gmsh input";
                return {};
            }
        }
        Input::readInputFile(inputPath.string());
        materials_ = Input::getMaterialList();
        bodies_ = Input::getBodyList(&materials_);
        EXPECT_EQ(materials_.size(), 1u);
        EXPECT_EQ(bodies_.size(), 1u);
        if (materials_.size() != 1 || bodies_.size() != 1) {
            return {};
        }
        EXPECT_NE(dynamic_cast<BodyGmsh*>(bodies_[0]), nullptr);

        Mesh mesh;
        mesh.setCellDimension(cellLength, cellLength, cellLength);
        bodies_[0]->create(mesh, materials_[0]);

        ParticleTotals totals;
        for (const Particle* particle : *bodies_[0]->getParticles()) {
            ++totals.count;
            if (dynamic_cast<const ParticleMixture*>(particle)) {
                ++totals.mixtureCount;
            }
            totals.mass += particle->getMass();
            totals.currentVolume += particle->getCurrentVolume();
        }
        return totals;
    }

    void expectUnitCube(const ParticleTotals& totals, std::size_t count) {
        EXPECT_EQ(totals.count, count);
        EXPECT_NEAR(totals.mass, 2000.0, 1e-9);
        EXPECT_NEAR(totals.currentVolume, 1.0, 1e-12);
    }

private:
    void clearBodiesAndMaterials() {
        for (Body* body : bodies_) {
            for (Particle* particle : *body->getParticles()) {
                delete particle;
            }
            delete body;
        }
        bodies_.clear();
        for (Material* material : materials_) {
            delete material;
        }
        materials_.clear();
    }

    bool previousTwoPhase_ = false;
    fs::path temporaryDirectory_;
    std::vector<Material*> materials_;
    std::vector<Body*> bodies_;
};

TEST_F(BodyGmshTest, FixtureContainsSixTetrahedraWithUnitVolume) {
    GMSH::GmshMesh mesh;
    ASSERT_TRUE(GMSH::GmshMeshReader::load(unitCubeMesh().string(), mesh));
    ASSERT_EQ(mesh.nodes.size(), 8u);
    ASSERT_EQ(mesh.elements.size(), 6u);

    std::unordered_map<int, Vector3d> nodes;
    for (const auto& node : mesh.nodes) {
        EXPECT_TRUE(node.x == 0.0 || node.x == 1.0);
        EXPECT_TRUE(node.y == 0.0 || node.y == 1.0);
        EXPECT_TRUE(node.z == 0.0 || node.z == 1.0);
        nodes.emplace(node.tag, Vector3d(node.x, node.y, node.z));
    }

    double volume = 0.0;
    for (const auto& element : mesh.elements) {
        ASSERT_EQ(element.type, 4);
        ASSERT_EQ(element.nodeTags.size(), 4u);
        for (int tag : element.nodeTags) {
            ASSERT_EQ(nodes.count(tag), 1u);
        }
        const Vector3d& a = nodes.at(element.nodeTags[0]);
        const Vector3d& b = nodes.at(element.nodeTags[1]);
        const Vector3d& c = nodes.at(element.nodeTags[2]);
        const Vector3d& d = nodes.at(element.nodeTags[3]);
        const double elementVolume = std::abs((b-a).cross(c-a).dot(d-a)) / 6.0;
        EXPECT_NEAR(elementVolume, 1.0 / 6.0, 1e-12);
        volume += elementVolume;
    }
    EXPECT_NEAR(volume, 1.0, 1e-12);
}

TEST_F(BodyGmshTest, DefaultDistributionIsBarycentricAndConservesVolume) {
    const ParticleTotals totals = createCube("");
    expectUnitCube(totals, 6);
    EXPECT_EQ(totals.mixtureCount, 0u);
}

TEST_F(BodyGmshTest, ExplicitBarycentricConservesMassAndCurrentVolume) {
    expectUnitCube(createCube("barycentric"), 6);
}

TEST_F(BodyGmshTest, GaussianConservesMassAndCurrentVolume) {
    expectUnitCube(createCube("gaussian"), 24);
}

TEST_F(BodyGmshTest, GaussianMassAndCurrentVolumeAreIndependentOfBackgroundCellSize) {
    const ParticleTotals coarse = createCube("gaussian", 1.0);
    const ParticleTotals fine = createCube("gaussian", 0.5);
    expectUnitCube(coarse, 24);
    expectUnitCube(fine, 24);
    EXPECT_NEAR(fine.mass, coarse.mass, 1e-9);
    EXPECT_NEAR(fine.currentVolume, coarse.currentVolume, 1e-12);
}

TEST_F(BodyGmshTest, TwoPhaseZeroPorosityConservesMassAndCurrentVolume) {
    const ParticleTotals barycentric = createCube("barycentric", 1.0, true, 0.0);
    const ParticleTotals gaussian = createCube("gaussian", 1.0, true, 0.0);
    expectUnitCube(barycentric, 6);
    expectUnitCube(gaussian, 24);
    EXPECT_EQ(barycentric.mixtureCount, barycentric.count);
    EXPECT_EQ(gaussian.mixtureCount, gaussian.count);
}

TEST_F(BodyGmshTest, TwoPhaseSolidMassIsIndependentOfDistributionAtNonzeroPorosity) {
    const ParticleTotals barycentric = createCube("barycentric", 1.0, true, 0.3);
    const ParticleTotals gaussian = createCube("gaussian", 1.0, true, 0.3);
    ASSERT_EQ(barycentric.count, 6u);
    ASSERT_EQ(gaussian.count, 24u);
    EXPECT_EQ(barycentric.mixtureCount, barycentric.count);
    EXPECT_EQ(gaussian.mixtureCount, gaussian.count);
    ASSERT_TRUE(std::isfinite(barycentric.mass));
    ASSERT_GT(barycentric.mass, 0.0);
    EXPECT_NEAR(gaussian.mass, barycentric.mass, 1e-9);
}

} // namespace
