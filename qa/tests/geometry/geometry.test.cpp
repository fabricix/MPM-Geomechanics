#include <gtest/gtest.h>

#include "Geometry.h"

TEST(Geometry, Bounding_Box)
{
  const std::vector<Vector3d> points{
      Vector3d(2.0, -1.0, 4.0),
      Vector3d(-3.0, 5.0, 0.0),
      Vector3d(1.0, 2.0, -2.0)};
  std::vector<Vector3d> box;

  Geometry::getBoundingBox(points, box);

  ASSERT_EQ(box.size(), 2u);
  EXPECT_TRUE(box[0].isApprox(Vector3d(-3.0, -1.0, -2.0)));
  EXPECT_TRUE(box[1].isApprox(Vector3d(2.0, 5.0, 4.0)));
}

TEST(Geometry, Point_Inside_Box)
{
  const Vector3d lower(0.0, 0.0, 0.0);
  const Vector3d upper(1.0, 2.0, 3.0);

  EXPECT_TRUE(Geometry::getInsideBox(lower, upper, Vector3d(0.5, 1.0, 2.0)));
  EXPECT_TRUE(Geometry::getInsideBox(lower, upper, lower));
  EXPECT_TRUE(Geometry::getInsideBox(lower, upper, upper));
  EXPECT_FALSE(Geometry::getInsideBox(lower, upper, Vector3d(-0.1, 1.0, 2.0)));
  EXPECT_FALSE(Geometry::getInsideBox(lower, upper, Vector3d(0.5, 1.0, 3.1)));
}

TEST(Geometry, Point_Inside_Polygon_On_Each_Extrusion_Plane)
{
  const std::vector<Vector3d> polygonXY{
      Vector3d(0.0, 0.0, 0.0),
      Vector3d(2.0, 0.0, 0.0),
      Vector3d(2.0, 2.0, 0.0),
      Vector3d(0.0, 2.0, 0.0)};
  const std::vector<Vector3d> polygonXZ{
      Vector3d(0.0, 0.0, 0.0),
      Vector3d(0.0, 0.0, 2.0),
      Vector3d(2.0, 0.0, 2.0),
      Vector3d(2.0, 0.0, 0.0)};
  const std::vector<Vector3d> polygonYZ{
      Vector3d(0.0, 0.0, 0.0),
      Vector3d(0.0, 2.0, 0.0),
      Vector3d(0.0, 2.0, 2.0),
      Vector3d(0.0, 0.0, 2.0)};

  EXPECT_TRUE(Geometry::getInsidePolygon(polygonXY, Vector3d(1.0, 1.0, 4.0), "z"));
  EXPECT_FALSE(Geometry::getInsidePolygon(polygonXY, Vector3d(3.0, 1.0, 4.0), "z"));
  EXPECT_TRUE(Geometry::getInsidePolygon(polygonXZ, Vector3d(1.0, 4.0, 1.0), "y"));
  EXPECT_FALSE(Geometry::getInsidePolygon(polygonXZ, Vector3d(3.0, 4.0, 1.0), "y"));
  EXPECT_TRUE(Geometry::getInsidePolygon(polygonYZ, Vector3d(4.0, 1.0, 1.0), "x"));
  EXPECT_FALSE(Geometry::getInsidePolygon(polygonYZ, Vector3d(4.0, 3.0, 1.0), "x"));
  EXPECT_FALSE(Geometry::getInsidePolygon(polygonXY, Vector3d(1.0, 1.0, 0.0), "invalid"));
}
