#include "Mesh/STLReader.h"
#include <iostream>

void print_stl_mesh_info(STLReader reader)
{
    // print the number of triangles
    const auto& triangles = reader.getTriangles();
    std::cout << "Number of triangles: " << triangles.size() << std::endl;

    // print the vertexes of the triangles
    for (const auto& tri : triangles) {
        std::cout << "Vertex 1: (" << tri.v1.x() << ", " << tri.v1.y() << ", " << tri.v1.z() << ")" << std::endl;
        std::cout << "Vertex 2: (" << tri.v2.x() << ", " << tri.v2.y() << ", " << tri.v2.z() << ")" << std::endl;
        std::cout << "Vertex 3: (" << tri.v3.x() << ", " << tri.v3.y() << ", " << tri.v3.z() << ")" << std::endl;
    }
}

int main(int argc, char* argv[])
{    
    std::string filename = argv[1];
    
    std::cout <<"Filename:" << filename << std::endl;

    STLReader reader;

    if (!reader.read(filename)) {
        std::cerr << "Error reading mesh ..." << std::endl;
        return -1;
    }
    
    print_stl_mesh_info(reader);
    
    return 0;
}