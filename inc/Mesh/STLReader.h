#ifndef STL_READER_H
#define STL_READER_H

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include <string>
#include <vector>

struct Triangle {
    Vector3d normal;
    Vector3d v1, v2, v3;
};

class STLReader {
public:
    bool read(const std::string& filename);
    const std::vector<Triangle>& getTriangles() const;

private:
    std::vector<Triangle> triangles;
    bool readASCII(std::ifstream& file);
    bool readBinary(std::ifstream& file);
};

#endif
