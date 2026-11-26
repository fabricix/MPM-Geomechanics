// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CELL_H_
#define CELL_H_

#include <array>
#include <cstddef>
#include <vector>

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include "Node.h"

class Cell {

    public:
        // Local positions relative to the cell's minimum and maximum coordinates.
        // Edge and face nodes are stored separately from the eight corner nodes.
        enum class HangingNodePosition : std::size_t {
            EdgeX_YMin_ZMin, EdgeX_YMax_ZMin, EdgeX_YMin_ZMax, EdgeX_YMax_ZMax,
            EdgeY_XMin_ZMin, EdgeY_XMax_ZMin, EdgeY_XMin_ZMax, EdgeY_XMax_ZMax,
            EdgeZ_XMin_YMin, EdgeZ_XMax_YMin, EdgeZ_XMin_YMax, EdgeZ_XMax_YMax,
            FaceXMin, FaceXMax, FaceYMin, FaceYMax, FaceZMin, FaceZMax,
            Count
        };

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

    /// Store a non-owning pointer to a hanging node, or nullptr if absent.
    inline void setHangingNode(HangingNodePosition position, Node* node) {
        hangingNodes.at(static_cast<std::size_t>(position)) = node;
    }

    /// Return the hanging node at this local position, or nullptr if absent.
    inline Node* getHangingNode(HangingNodePosition position) const {
        return hangingNodes.at(static_cast<std::size_t>(position));
    }

    private:
        std::vector<Node*> nodes;
        std::array<Node*, static_cast<std::size_t>(HangingNodePosition::Count)> hangingNodes{};
        double volume;
        int id;
};

#endif /* CELL_H_ */