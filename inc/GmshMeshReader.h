// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

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

    // helper to encode (dim, tag) into a 64-bit key
    inline long long entityKey(int dim, int tag) {
        return ( (static_cast<long long>(dim) << 32) | (tag & 0xffffffff) );
    }

    // entity structure (defines relation between entities and physical groups)
    struct Entity {
        int dim = 0;                     // 0=point, 1=curve, 2=surface, 3=volume
        int tag = 0;                     // entityTag (matches Element::entityTag)
        std::vector<int> physicals;      // list of physical group IDs attached
    };

    // complete Gmsh mesh structure
    struct GmshMesh {
        std::vector<Node> nodes;
        std::vector<Element> elements;
        std::unordered_map<int, PhysicalGroup> physicals; // key = physId
        std::unordered_map<long long, Entity> entities;
    };

    // File reader .msh (ASCII)
    class GmshMeshReader {
    public:
        static bool load(const std::string &filename, GmshMesh &mesh);
    private:
        static void readPhysicalNames(std::ifstream &in, GmshMesh &mesh);
        static void readEntities(std::ifstream &in, GmshMesh &mesh);
        static void readNodes(std::ifstream &in, GmshMesh &mesh);
        static void readElements(std::ifstream &in, GmshMesh &mesh);
    };
}