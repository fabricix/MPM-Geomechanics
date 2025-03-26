// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CELL_H_
#define CELL_H_

#include <vector>

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include "Node.h"

class Cell {

    public:
        
        Cell() : volume(0.0), id(0) {}

        void computeVolume() {

            if (nodes.size() != 8) {
                volume = 0.0;
                return;
            }
        
            // base vectors of the parallelepiped 
            Vector3d v1 = nodes[1]->getCoordinates() - nodes[0]->getCoordinates(); // X direction
            Vector3d v2 = nodes[2]->getCoordinates() - nodes[0]->getCoordinates(); // Y direction
            Vector3d v3 = nodes[4]->getCoordinates() - nodes[0]->getCoordinates(); // Z direction
        
            // cross product of two base vectors
            volume = std::abs(v1.cross(v2).dot(v3));

            // set id of the cell with the id of the first node
            this->id = nodes[0]->getId();
    }

    inline double getVolume() const { return volume; };
    inline int getId() const { return id; };
    inline std::vector<Node*> getNodes() const { return nodes; };
    inline void setNodes(const std::vector<Node*>& nodes) { this->nodes = nodes; };

    private:
        std::vector<Node*> nodes;
        double volume;
        int id;
};

#endif /* CELL_H_ */