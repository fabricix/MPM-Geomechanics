#include <gtest/gtest.h>

#include <fstream>

#include "Input.h"
#include "Interpolation.h"
#include "Mesh/Mesh.h"
#include "Model.h"
#include "Seismic.h"
#include "TestTemporaryDirectory.h"

class SeismicTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    Seismic::disableSeismicAnalysis();
    Seismic::setSeismicAnalysis({true, false, ""});
    ModelSetup::setSeismicAnalysisActive(true);
    auto& data = Seismic::getSeismicData();
    data.time = {0.0, 1.0};
    data.acceleration = {Vector3d(0.0, 0.0, 0.0), Vector3d(4.0, 2.0, -2.0)};
  }

  void TearDown() override
  {
    Seismic::disableSeismicAnalysis();
    ModelSetup::setSeismicAnalysisActive(false);
  }
};

TEST_F(SeismicTest, ReadsTimeSeriesAndInterpolatesWithinRecord)
{
  TestTemporaryDirectory directory;
  const auto recordFile = directory.path() / "record.csv";
  {
    std::ofstream record(recordFile);
    ASSERT_TRUE(record.is_open());
    record << "time,ax,ay,az\n0,0,0,0\n1,4,2,-2\n";
  }

  const SeismicData parsed = Input::readSeismicData(recordFile.string(), true);
  ASSERT_EQ(parsed.time.size(), 2u);
  ASSERT_EQ(parsed.acceleration.size(), 2u);
  EXPECT_TRUE(parsed.acceleration[1].isApprox(Vector3d(4.0, 2.0, -2.0)));

  EXPECT_TRUE(Interpolation::interpolateVector(parsed.time, parsed.acceleration, -0.1)
                  .isApprox(Vector3d::Zero()));
  EXPECT_TRUE(Interpolation::interpolateVector(parsed.time, parsed.acceleration, 0.0)
                  .isApprox(Vector3d::Zero()));
  EXPECT_TRUE(Interpolation::interpolateVector(parsed.time, parsed.acceleration, 0.5)
                  .isApprox(Vector3d(2.0, 1.0, -1.0)));

  Seismic::getSeismicData() = parsed;
  Seismic::updateSeismicVectors(0.5, 0.1);
  EXPECT_TRUE(Seismic::getAcceleration().isApprox(Vector3d(2.0, 1.0, -1.0)));
  EXPECT_TRUE(Seismic::getAccumulatedVelocity().isApprox(Vector3d(0.2, 0.1, -0.1)));
}

TEST_F(SeismicTest, MarksNodesAndAppliesVelocityOnlyToActiveMarkedNodes)
{
  Mesh mesh;
  mesh.setNumCells(1, 1, 1);
  mesh.setCellDimension(1.0, 1.0, 1.0);
  mesh.createGrid();

  auto* nodes = mesh.getNodes();
  for (auto* node : *nodes)
    node->setDistanceLevelSet(1.0);

  const int markedActiveId = 0;
  const int markedInactiveId = 1;
  nodes->at(markedActiveId)->setDistanceLevelSet(0.05);
  nodes->at(markedActiveId)->setActive(true);
  nodes->at(markedInactiveId)->setDistanceLevelSet(-0.05);
  nodes->at(markedInactiveId)->setActive(false);

  Seismic::markSeismicNodes(0.1, &mesh);
  ASSERT_TRUE(Seismic::isSeismicNode(markedActiveId));
  ASSERT_TRUE(Seismic::isSeismicNode(markedInactiveId));
  EXPECT_FALSE(Seismic::isSeismicNode(2));

  Seismic::updateSeismicVectors(0.5, 0.1);
  Seismic::applySeismicVelocityMarkedSTLNodes(&mesh);

  EXPECT_TRUE(nodes->at(markedActiveId)->getVelocity().isApprox(Vector3d(0.2, 0.1, -0.1)));
  EXPECT_TRUE(nodes->at(markedInactiveId)->getVelocity().isApprox(Vector3d::Zero()));
}

TEST_F(SeismicTest, DisablesAnalysisAtEndOfRecord)
{
  EXPECT_TRUE(Interpolation::interpolateVector(
                  Seismic::getSeismicData().time,
                  Seismic::getSeismicData().acceleration,
                  1.0).isApprox(Vector3d::Zero()));
  EXPECT_FALSE(Seismic::getSeismicAnalysis().isActive);
  EXPECT_FALSE(ModelSetup::getSeismicAnalysisActive());
  EXPECT_TRUE(Seismic::getSeismicData().time.empty());
}
