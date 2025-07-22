// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once

#include "Mesh/Mesh.h"

#include <Eigen/Dense>
#include <vector>
#include <string>

/**
 * @brief Seismic data structure to manage acceleration and velocity of an earthquake record.
 * 
 */
struct SeismicData
{
    std::vector<double> time;
    std::vector<Eigen::Vector3d> acceleration;
};

struct SeismicAnalysis 
{
	bool isActive = false;
	bool hasHeader = true;
	std::string filename = "";
};

namespace Seismic
{
    void setSeismicData(); // set acceleration data from file
    SeismicData& getSeismicData(); // get seismic acceleration data
    SeismicAnalysis& getSeismicAnalysis(); // get seismic analysis information
    void setSeismicAnalysis(const SeismicAnalysis& info); // set seismic analysis information
    void applySeismicVelocity(double currentTime, double dt, Mesh* mesh); // apply seismic velocity to nodes
    void markSeismicNodes(double epsilon, Mesh* mesh); // mark seismic nodes based on distance from level set
    const std::vector<int>& getSeismicNodeIndices(); // get indices of seismic nodes
    bool isSeismicNode(int nodeId); // check if a node is a seismic node
    Eigen::Vector3d& getAccumulatedVelocity(); // get accumulated velocity
}