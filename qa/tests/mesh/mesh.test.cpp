#include <gtest/gtest.h>

#include "Mesh/Mesh.h"

TEST(Mesh, GridDimensionsAndDomainQueries)
{
  Mesh mesh;
  mesh.setNumCells(2, 1, 3);
  mesh.setCellDimension(0.5, 2.0, 1.5);
  mesh.setOrigin(-1.0, 2.0, -3.0);
  mesh.createGrid();

  EXPECT_EQ(mesh.getNumGhosts(), 2);
  EXPECT_EQ(mesh.getTotalCells().x(), 6);
  EXPECT_EQ(mesh.getTotalCells().y(), 5);
  EXPECT_EQ(mesh.getTotalCells().z(), 7);
  EXPECT_EQ(mesh.getNumNodes(), 7u * 6u * 8u);
  EXPECT_EQ(mesh.getCells()->size(), 6u * 5u * 7u);

  EXPECT_TRUE(mesh.getMinLimits().isApprox(Vector3d(-1.0, 2.0, -3.0)));
  EXPECT_TRUE(mesh.getMaxLimits().isApprox(Vector3d(0.0, 4.0, 1.5)));

  EXPECT_TRUE(mesh.getIsInsideMesh(Vector3d(-1.0, 2.0, -3.0)));
  EXPECT_TRUE(mesh.getIsInsideMesh(Vector3d(0.0, 4.0, 1.5)));
  EXPECT_TRUE(mesh.getIsInsideMesh(Vector3d(-0.5, 3.0, 0.0)));
  EXPECT_FALSE(mesh.getIsInsideMesh(Vector3d(-1.01, 3.0, 0.0)));
  EXPECT_FALSE(mesh.getIsInsideMesh(Vector3d(0.0, 4.0, 1.51)));
}