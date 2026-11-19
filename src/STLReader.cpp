// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Mesh/STLReader.h"
#include <array>
#include <cstdint>
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
 
    file.seekg(0, std::ios::end);
    const std::streamoff fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::array<char, 80> header{};
    file.read(header.data(), static_cast<std::streamsize>(header.size()));
    const std::streamsize headerSize = file.gcount();

    if (fileSize >= 84) {
        std::uint32_t numTriangles = 0;
        file.clear();
        file.seekg(80, std::ios::beg);
        file.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));
        const std::uint64_t expectedSize =
            84 + static_cast<std::uint64_t>(numTriangles) * 50;
        if (file && expectedSize == static_cast<std::uint64_t>(fileSize)) {
            file.seekg(80, std::ios::beg);
            return readBinary(file);
        }
    }

    std::string headerStr(header.data(), static_cast<size_t>(headerSize));
    const size_t firstNonWhitespace = headerStr.find_first_not_of(" \t\r\n");
    if (firstNonWhitespace != std::string::npos &&
        headerStr.compare(firstNonWhitespace, 5, "solid") == 0) {
        file.close();
        file.open(filename); // open it in text mode
        return readASCII(file);
    }

    std::cerr << "Invalid or unsupported STL file: " << filename << std::endl;
    return false;
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
    return !triangles.empty();
}

bool STLReader::readBinary(std::ifstream& file) {
    std::uint32_t numTriangles = 0;
    file.read(reinterpret_cast<char*>(&numTriangles), sizeof(uint32_t));
    if (!file) {
        return false;
    }

    std::vector<Triangle> parsedTriangles;
    if (numTriangles > parsedTriangles.max_size()) {
        return false;
    }
    parsedTriangles.reserve(numTriangles);

    for (uint32_t i = 0; i < numTriangles; ++i) {
        Triangle triangle;
        auto readVector = [&file](Vector3d& value) {
            std::array<float, 3> coordinates{};
            file.read(reinterpret_cast<char*>(coordinates.data()), sizeof(coordinates));
            if (!file) {
                return false;
            }
            value = Vector3d(coordinates[0], coordinates[1], coordinates[2]);
            return true;
        };
        if (!readVector(triangle.normal) ||
            !readVector(triangle.v1) ||
            !readVector(triangle.v2) ||
            !readVector(triangle.v3)) {
            return false;
        }

        std::uint16_t attributeByteCount = 0;
        file.read(reinterpret_cast<char*>(&attributeByteCount), sizeof(attributeByteCount));
        if (!file) {
            return false;
        }
        parsedTriangles.push_back(triangle);
    }

    triangles = std::move(parsedTriangles);
    return true;
}

const std::vector<Triangle>& STLReader::getTriangles() const {
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