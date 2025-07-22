// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once

#include "Mesh/Mesh.h"

#include <Eigen/Dense>
#include <vector>
#include <string>

/**
 * @struct SeismicData
 * @brief Stores seismic time series data.
 *
 * This structure contains the time steps and 3D acceleration values
 * for a seismic record.
 *
 * @var SeismicData::time
 *   Vector of time values (s).
 *
 * @var SeismicData::acceleration
 *   Vector of 3D acceleration values (m/s2) for each time step.
 *
 */
struct SeismicData
{
    std::vector<double> time;
    std::vector<Eigen::Vector3d> acceleration;
};

/**
 * @struct SeismicAnalysis
 * @brief Structure to hold seismic analysis information.
 * 
 * This structure contains information about whether seismic analysis is active,
 * whether the seismic data file has a header, and the filename of the seismic data.
 * 
 * @var SeismicAnalysis::isActive
 *   Boolean indicating if seismic analysis is active.
 * @var SeismicAnalysis::hasHeader
 *   Boolean indicating if the seismic data file has a header.
 */
struct SeismicAnalysis 
{
	bool isActive = false;
	bool hasHeader = true;
	std::string filename = "";
};

namespace Seismic
{
    void setSeismicData();
    SeismicData& getSeismicData(); // get seismic acceleration data
    SeismicAnalysis& getSeismicAnalysis(); // get seismic analysis information
    void setSeismicAnalysis(const SeismicAnalysis& info); // set seismic analysis information
    void applySeismicVelocity(double currentTime, double dt, Mesh* mesh); // apply seismic velocity to nodes
    void markSeismicNodes(double epsilon, Mesh* mesh); // mark seismic nodes based on distance from level set
    const std::vector<int>& getSeismicNodeIndices(); // get indices of seismic nodes
    bool isSeismicNode(int nodeId); // check if a node is a seismic node
    Eigen::Vector3d& getAccumulatedVelocity(); // get accumulated velocity
}