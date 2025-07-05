// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Mesh/STLSeismicLoading.h"
#include "Mesh/Mesh.h"
#include <cmath>

STLSeismicLoading::STLSeismicLoading(Mesh* mesh)
    : mesh_(mesh)
{}

void STLSeismicLoading::markSeismicNodes(double epsilon)
{
    std::vector<Node*>* nodes = mesh_->getNodes();
    seismicNodeIndices_.clear();

    for (int i = 0; i < static_cast<int>(nodes->size()); ++i) {
        Node* node = nodes->at(i);
        double d = std::abs(node->getDistanceLevelSet());

        if (d < epsilon) {
            seismicNodeIndices_.push_back(i);
        }
    }
}

void STLSeismicLoading::applySeismicForce(double currentTime)
{
    const Eigen::Vector3d a = Interpolation::interpolateVector(
        Seismic::getSeismicData().time,
        Seismic::getSeismicData().acceleration,
        currentTime
    );

    std::vector<Node*>* nodes = mesh_->getNodes();

    #pragma omp parallel for
    for (int i = 0; i < static_cast<int>(seismicNodeIndices_.size()); ++i) {
        int nodeId = seismicNodeIndices_[i];
        Node* node = nodes->at(nodeId);
        if (!node->getActive()) continue;

        Eigen::Vector3d seismicForce = node->getMass() * a;
        node->addExternalForce(seismicForce);
    }
}