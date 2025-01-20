#ifndef STL_READER_H
#define STL_READER_H

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include <string>
#include <vector>

/// @brief Struct representing a triangle
struct Triangle {
    Vector3f normal;
    Vector3f v1, v2, v3;
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
