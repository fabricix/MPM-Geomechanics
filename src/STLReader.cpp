#include "Mesh/STLReader.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool STLReader::read(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening STL file: " << filename << std::endl;
        return false;
    }

    // read format
    char header[80];
    file.read(header, 80);

    // determine if it is ASCII or binary
    std::string headerStr(header, 80);
    if (headerStr.find("solid") == 0) {
        file.close();
        file.open(filename); // open it in text mode
        return readASCII(file);
    } else {
        return readBinary(file);
    }
}

bool STLReader::readASCII(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string word;
        ss >> word;

        if (word == "facet") {
            Triangle triangle;
            ss >> word >> triangle.normal.x() >> triangle.normal.y() >> triangle.normal.z();

            // Leer los vértices
            std::getline(file, line);  // outer loop
            std::getline(file, line);
            std::sscanf(line.c_str(), " vertex %f %f %f", &triangle.v1.x(), &triangle.v1.y(), &triangle.v1.z());

            std::getline(file, line);
            std::sscanf(line.c_str(), " vertex %f %f %f", &triangle.v2.x(), &triangle.v2.y(), &triangle.v2.z());

            std::getline(file, line);
            std::sscanf(line.c_str(), " vertex %f %f %f", &triangle.v3.x(), &triangle.v3.y(), &triangle.v3.z());

            triangles.push_back(triangle);

            std::getline(file, line);  // endloop
            std::getline(file, line);  // endfacet
        }
    }
    return true;
}

bool STLReader::readBinary(std::ifstream& file) {
    
    uint32_t numTriangles;
    
    file.read(reinterpret_cast<char*>(&numTriangles), sizeof(uint32_t));

    for (uint32_t i = 0; i < numTriangles; ++i) {
        Triangle triangle;
        file.read(reinterpret_cast<char*>(&triangle.normal), sizeof(Vector2d));
        file.read(reinterpret_cast<char*>(&triangle.v1), sizeof(Vector2d));
        file.read(reinterpret_cast<char*>(&triangle.v2), sizeof(Vector2d));
        file.read(reinterpret_cast<char*>(&triangle.v3), sizeof(Vector2d));
        file.ignore(2); // Ignore attributes for now
        triangles.push_back(triangle);
    }
    return true;
}

const std::vector<Triangle>& STLReader::getTriangles() const {
    return triangles;
}