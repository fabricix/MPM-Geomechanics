// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef STL_READER_H
#define STL_READER_H

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include <string>
#include <vector>

/// @brief Struct representing a triangle
struct Triangle {
    
    Vector3d normal; ///< Normal vector of the triangle
    Vector3d v1, v2, v3; ///< Vertices of the triangle

    /// @brief Get the first vertex of the triangle
    /// @return The first vertex (v1)
    Vector3d getVertex1() const {
        return v1;
    }

    /// @brief Get the second vertex of the triangle
    /// @return The second vertex (v2)
    const Vector3d& getVertex2() const {
        return v2;
    }

    /// @brief Get the third vertex of the triangle
    /// @return The third vertex (v3)
    const Vector3d& getVertex3() const {
        return v3;
    }

    /// @brief Get the normal of the triangle
    /// @return The normal vector of the triangle
    const Vector3d& getNormal() const {
        return normal;
    }

    /// @brief Calculate the centroid of the triangle
    /// @return The centroid of the triangle
    Vector3d getCentroid() const {
        return (v1 + v2 + v3) / 3.0f;
    }

    /// @brief Calculate the area of the triangle
    /// @return The area of the triangle
    double getArea() const {
        return 0.5f * ((v2 - v1).cross(v3 - v1)).norm();
    }
};

/// @brief Class to read STL files
class STLReader {

public:
    /// @brief Read the STL file
    bool read(const std::string& filename);
    
    /// @brief Get the triangles of the STL mesh 
    /// @return Vector containing the triangles
    const std::vector<Triangle>& getTriangles() const;

    /// @brief Remove triangles that have all vertices outside the specified bounding box
    /// @param min Bounding box minimum (x_min, y_min, z_min)
    /// @param max Bounding box maximum (x_max, y_max, z_max)
    void removeTrianglesOutsideLimits(const Vector3d& min, const Vector3d& max);

    /// @brief Recalculate the normals of the triangles
    /// @details This function recalculates the normals of the triangles based on their vertices.
    void recalculateNormals();

    /// @brief Write the STL file
    /// @param output_filename The name of the output STL file
    bool writeSTL(const std::string& output_filename) const;

    /// @brief Update the STL mesh by applying a displacement to all vertices
    void updateSTLMesh(const Eigen::Vector3d& displacement);

    /// @brief Get the original vertices of the STL mesh
    void storeOriginalVertices();

private:
    
    /// @brief Vector containing the triangles
    std::vector<Triangle> triangles;

    /// @brief Vector containing the original vertices of the STL mesh
    /// @details This vector is used to store the original vertices of the STL mesh before any
    std::vector<Eigen::Vector3d> originalVertices;

    /// @brief Read the ASCII STL file
    bool readASCII(std::ifstream& file);
    
    /// @brief Read the binary STL file
    bool readBinary(std::ifstream& file);
};

#endif
