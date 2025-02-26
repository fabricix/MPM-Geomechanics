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

private:
    
    /// @brief Vector containing the triangles
    std::vector<Triangle> triangles;

    /// @brief Read the ASCII STL file
    bool readASCII(std::ifstream& file);
    
    /// @brief Read the binary STL file
    bool readBinary(std::ifstream& file);
};

#endif
