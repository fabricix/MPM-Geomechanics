// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace GMSH {

    // node structure for Gmsh mesh
    struct Node {
        int tag;
        double x, y, z;
    };

    // element structure for Gmsh mesh (see Gmsh documentation for types)
    struct Element {
        int tag;
        int type; // (2=TRI3, 3=QUAD4, 4=TET4, 5=HEX8, etc.)
        int entityTag;
        std::vector<int> nodeTags;
    };

    // physical group structure for Gmsh mesh
    struct PhysicalGroup {
        int dim;
        int id;
        std::string name;
    };

    // complete Gmsh mesh structure
    struct GmshMesh {
        std::vector<Node> nodes;
        std::vector<Element> elements;
        std::unordered_map<int, PhysicalGroup> physicals; // key = physId
    };

    // File reader .msh (ASCII)
    class GmshMeshReader {
    public:
        static bool load(const std::string &filename, GmshMesh &mesh);
    private:
        static void readPhysicalNames(std::ifstream &in, GmshMesh &mesh);
        static void readEntities(std::ifstream &in);
        static void readNodes(std::ifstream &in, GmshMesh &mesh);
        static void readElements(std::ifstream &in, GmshMesh &mesh);
    };
}