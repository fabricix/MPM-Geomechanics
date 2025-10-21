// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "GmshMeshReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace GMSH {

bool GmshMeshReader::load(const std::string &filename, GmshMesh &mesh) {
    
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Can't open the file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("$PhysicalNames", 0) == 0) {
            readPhysicalNames(in, mesh);
        } else if (line.rfind("$Entities", 0) == 0) {
            readEntities(in, mesh);
        } else if (line.rfind("$Nodes", 0) == 0) {
            readNodes(in, mesh);
        } else if (line.rfind("$Elements", 0) == 0) {
            readElements(in, mesh);
        }
    }

    std::cout << " Read gmsh : " 
              << mesh.nodes.size() << " nodes, "
              << mesh.elements.size() << " elements." << std::endl;
    return true;
}

void GmshMeshReader::readPhysicalNames(std::ifstream &in, GmshMesh &mesh) {
    std::string line;
    std::getline(in, line);
    int nPhys = std::stoi(line);

    for (int i = 0; i < nPhys; ++i) {
        std::getline(in, line);
        std::istringstream iss(line);
        int dim, id;
        std::string name;
        iss >> dim >> id >> name;
        if (!name.empty() && name.front() == '\"')
            name = name.substr(1, name.size() - 2);
        mesh.physicals[id] = {dim, id, name};
    }
    std::getline(in, line); // $EndPhysicalNames
}

void GmshMeshReader::readEntities(std::ifstream &in, GmshMesh &mesh)
{
    std::string line;

    //  header: numPoints numCurves numSurfaces numVolumes
    if (!std::getline(in, line)) return;
    int np=0, nc=0, ns=0, nv=0;
    {
        std::istringstream ss(line);
        ss >> np >> nc >> ns >> nv;
    }

    auto parseBlock = [&](int dim, int count) {
        for (int i = 0; i < count; ++i) {
            if (!std::getline(in, line)) break;
            std::istringstream ss(line);

            Entity E; E.dim = dim;

            // tag
            if (!(ss >> E.tag)) continue;

            // coords / bounding box
            if (dim == 0) {
                // points: x y z
                double x=0,y=0,z=0;
                if (!(ss >> x >> y >> z)) continue;
            } else {
                // curve/surface/volume: xmin ymin zmin xmax ymax zmax
                double bbminmax[6]{};
                if (!(ss >> bbminmax[0] >> bbminmax[1] >> bbminmax[2]
                           >> bbminmax[3] >> bbminmax[4] >> bbminmax[5])) continue;
            }

            // nPhysicals and its ids
            int nPhys = 0;
            if (!(ss >> nPhys)) nPhys = 0;
            if (nPhys < 0) nPhys = 0; // robustez
            E.physicals.resize(nPhys);
            for (int j = 0; j < nPhys; ++j) {
                int pid = 0; ss >> pid; E.physicals[j] = pid;
            }

            // nBoundary + boundary tags (depend on dim)
            int nB = 0;
            if (ss >> nB) { for (int k = 0; k < nB; ++k) { int tmp=0; ss >> tmp; } }

            //nPartitions + partition tags + parent tag
            int nPart = 0;
            if (ss >> nPart) {
                for (int k = 0; k < nPart; ++k) { int tmp=0; ss >> tmp; }
                int parent = 0; ss >> parent;
            }

            mesh.entities[entityKey(dim, E.tag)] = std::move(E);
        }
    };

    parseBlock(0, np);
    parseBlock(1, nc);
    parseBlock(2, ns);
    parseBlock(3, nv);

    // jump to $EndEntities
    while (std::getline(in, line)) {
        if (line.rfind("$EndEntities", 0) == 0) break;
    }
}

void GmshMeshReader::readNodes(std::ifstream &in, GmshMesh &mesh) {

    std::string line;
    std::getline(in, line);
    int numBlocks, numNodes;
    {
        std::istringstream iss(line);
        iss >> numBlocks >> numNodes;
    }

    mesh.nodes.reserve(numNodes);
    for (int b = 0; b < numBlocks; ++b) {
        // header of the block: entityDim, entityTag, parametric, numNodesInBlock
        std::getline(in, line);
        int dim, tag, parametric, nInBlock;
        {
            std::istringstream iss(line);
            iss >> dim >> tag >> parametric >> nInBlock;
        }

        std::vector<int> nodeTags(nInBlock);
        for (int i = 0; i < nInBlock; ++i) {
            std::getline(in, line);
            nodeTags[i] = std::stoi(line);
        }

        for (int i = 0; i < nInBlock; ++i) {
            std::getline(in, line);
            std::istringstream iss(line);
            Node n;
            n.tag = nodeTags[i];
            iss >> n.x >> n.y >> n.z;
            mesh.nodes.push_back(n);
        }
    }

    // jump to $EndNodes
    while (std::getline(in, line))
        if (line.rfind("$EndNodes", 0) == 0) break;
}

void GmshMeshReader::readElements(std::ifstream &in, GmshMesh &mesh) {

    std::string line;
    std::getline(in, line);
    int numBlocks, numElems;
    {
        std::istringstream iss(line);
        iss >> numBlocks >> numElems;
    }

    mesh.elements.reserve(numElems);

    for (int b = 0; b < numBlocks; ++b) {
        // header of the block: entityDim, entityTag, elemType, numElemsInBlock
        std::getline(in, line);
        int dim, entityTag, elemType, nElemsInBlock;
        {
            std::istringstream iss(line);
            iss >> dim >> entityTag >> elemType >> nElemsInBlock;
        }

        for (int i = 0; i < nElemsInBlock; ++i) {
            std::getline(in, line);
            std::istringstream iss(line);
            Element e;
            e.entityTag = entityTag;
            e.type = elemType;
            iss >> e.tag;

            int nPerElem = 0;
            switch (elemType) {
                case 2: nPerElem = 3; break;  // TRI3
                case 3: nPerElem = 4; break;  // QUAD4
                case 4: nPerElem = 4; break;  // TET4
                case 5: nPerElem = 8; break;  // HEX8
                default: break;
            }

            e.nodeTags.resize(nPerElem);
            for (int j = 0; j < nPerElem; ++j)
                iss >> e.nodeTags[j];

            mesh.elements.push_back(std::move(e));
        }
    }

    // jump to $EndElements
    while (std::getline(in, line))
        if (line.rfind("$EndElements", 0) == 0) break;
}
}