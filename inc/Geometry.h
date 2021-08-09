/*
 * Geometry.h
 *
 *  Created on: 20 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Eigen/Core"
using Eigen::Vector3d;

#include<vector>
using std::vector;

#include <string>
using std::string;

/// \namespace Geometry
/// \brief Contain auxiliary functions related with geometric operations
namespace Geometry {

    /// \brief Verify in a point is inside of a polygon
    /// \param[in] polygon A list of points forming the polygon
    /// \param[in] point A point to verify if is inside the polygon
    /// \param[in] extrude_direction The normal vector of the plane where the polygon is defined
    /// \return True is the point is inside the polygon
    bool getInsidePolygon(const vector<Vector3d>& polygon, const Vector3d& point, string extrude_direction);

    /// \brief Calculate the bounding box of a group of points
    /// \param[in] points A group of points
    /// \param[out] box The bounding box formed by two points, the left-lower point and the right-upper point
    void getBoundingBox(const vector<Vector3d>& points, vector<Vector3d>& box);

    /// \brief Verify if a point is inside a box
    /// \param[in] p1 Left and lower point of the box
    /// \param[in] p2 Right and upper point of the box
    /// \param[in] position Point to be evaluated the function
    /// \return True if the point is inside the box, false otherwise
    bool getInsideBox(Vector3d p1, Vector3d p2, Vector3d position);
}

#endif /* GEOMETRY_H_ */