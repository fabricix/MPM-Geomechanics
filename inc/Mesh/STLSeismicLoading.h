// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once

#include "Mesh/Mesh.h"
#include "Seismic.h"
#include "Interpolation.h"

#include <unordered_set>

class STLSeismicLoading {
public:

    STLSeismicLoading(Mesh* mesh);

    // Mark nodes whose distanceLevelSet is within epsilon
    void markSeismicNodes(double epsilon);

    // Apply seismic acceleration to the marked nodes
    void applySeismicForce(double currentTime);

    // get seismic node indices
    const std::vector<int>& getSeismicNodeIndices() const {
        return seismicNodeIndices_;
    }

    // check if a node is a seismic node
    inline bool isSeismicNode(int nodeId) const {
        return seismicNodeSet_.count(nodeId) > 0;
    }
    
private:
    Mesh* mesh_;
    std::vector<int> seismicNodeIndices_;
    std::unordered_set<int> seismicNodeSet_;
};