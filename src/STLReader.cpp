// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Mesh/STLReader.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool STLReader::read(const std::string& filename)
{
    // open the file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening STL file: " << filename << std::endl;
        return false;
    }
 
    // read header to determine the format
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

bool STLReader::readASCII(std::ifstream& file) 
{
    // clear triangles
    triangles.clear();

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string word;
        ss >> word;

        if (word == "facet") 
        {
            Triangle triangle;
            ss >> word >> triangle.normal.x() >> triangle.normal.y() >> triangle.normal.z();

			// read vertex coordinates
            std::getline(file, line);  // outer loop
            std::getline(file, line);
            std::sscanf(line.c_str(), " vertex %lf %lf %lf", &triangle.v1.x(), &triangle.v1.y(), &triangle.v1.z());

            std::getline(file, line);
            std::sscanf(line.c_str(), " vertex %lf %lf %lf", &triangle.v2.x(), &triangle.v2.y(), &triangle.v2.z());

            std::getline(file, line);
            std::sscanf(line.c_str(), " vertex %lf %lf %lf", &triangle.v3.x(), &triangle.v3.y(), &triangle.v3.z());

            triangles.push_back(triangle);

            std::getline(file, line);  // end loop
            std::getline(file, line);  // end facet
        }
    }
    return true;
}

bool STLReader::readBinary(std::ifstream& file) {
    
    // clear triangles
    triangles.clear();

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

std::vector<Triangle>& STLReader::getTriangles() {
    return triangles;
}

bool is_inside(const Vector3d& v, const Vector3d& min, const Vector3d& max) {
    return (v.x() >= min.x() && v.x() <= max.x() &&
            v.y() >= min.y() && v.y() <= max.y() &&
            v.z() >= min.z() && v.z() <= max.z());
}

void STLReader::removeTrianglesOutsideLimits(const Vector3d& min, const Vector3d& max) {
    
    std::vector<Triangle> filtered;

    for (const auto& triangle : triangles) {
        if (is_inside(triangle.v1, min, max) ||
            is_inside(triangle.v2, min, max) ||
            is_inside(triangle.v3, min, max)) 
        {
            filtered.push_back(triangle);
        }
    }

    triangles = std::move(filtered);
}

void STLReader::recalculateNormals() {
    for (auto& triangle : triangles) {
        Vector3d edge1 = triangle.v2 - triangle.v1;
        Vector3d edge2 = triangle.v3 - triangle.v1;
        triangle.normal = edge1.cross(edge2).normalized();
    }
}

bool STLReader::writeSTL(const std::string& output_filename) const {
    std::ofstream file(output_filename);
    if (!file.is_open()) {
        std::cerr << "Error writing STL file: " << output_filename << std::endl;
        return false;
    }

    file << "solid exported_from_mpm_geomechanics\n";
    for (const auto& triangle : triangles) {
        Vector3d n = triangle.normal.normalized();
        file << "  facet normal " << n.x() << " " << n.y() << " " << n.z() << "\n";
        file << "    outer loop\n";
        file << "      vertex " << triangle.v1.x() << " " << triangle.v1.y() << " " << triangle.v1.z() << "\n";
        file << "      vertex " << triangle.v2.x() << " " << triangle.v2.y() << " " << triangle.v2.z() << "\n";
        file << "      vertex " << triangle.v3.x() << " " << triangle.v3.y() << " " << triangle.v3.z() << "\n";
        file << "    endloop\n";
        file << "  endfacet\n";
    }
    file << "endsolid exported_from_mpm_geomechanics\n";

    file.close();
    return true;
}

// Store the original vertices of the STL mesh
// This function initializes the originalVertices vector with the current vertices of the triangles.
void STLReader::storeOriginalVertices() {
    
    if (!originalVertices.empty()) return;

    originalVertices.reserve(triangles.size() * 3);
    for (const auto& tri : triangles) {
        originalVertices.push_back(tri.v1);
        originalVertices.push_back(tri.v2);
        originalVertices.push_back(tri.v3);
    }
}

// Update the STL mesh by applying a displacement to all vertices
// This function updates the vertices of the STL mesh by applying a displacement vector to each vertex.
void STLReader::updateSTLMesh(const Eigen::Vector3d& displacement)
{
    if (originalVertices.empty()) { storeOriginalVertices(); }

    for (size_t i = 0; i < triangles.size(); ++i) {
        triangles[i].v1 = originalVertices[i * 3 + 0] + displacement;
        triangles[i].v2 = originalVertices[i * 3 + 1] + displacement;
        triangles[i].v3 = originalVertices[i * 3 + 2] + displacement;
    }

    recalculateNormals();
}