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
    /**
     * @brief Loads seismic data from the file specified in the model configuration.
     *
     * This function reads the seismic input file defined in the JSON configuration
     * and stores the time and acceleration data into the global `SeismicData` structure.
     */
    void setSeismicData();

    /**
     * @brief Provides access to the loaded seismic data.
     * @return SeismicData& Reference to the seismic data structure.
     */
    SeismicData& getSeismicData();
    
    /**
     * @brief Provides access to the current seismic analysis parameters.
     * @return SeismicAnalysis& Reference to seismic analysis information.
     */
    SeismicAnalysis& getSeismicAnalysis();
    
    /**
     * @brief Sets the seismic analysis configuration.
     * @param info Seismic analysis information to set.
     */
    void setSeismicAnalysis(const SeismicAnalysis& info);
    
    /**
     * @brief Applies seismic velocity to the marked seismic nodes using STL mesh.
     *
     * @param currentTime Current simulation time (s).
     * @param dt Simulation time step (s).
     * @param mesh Pointer to the Eulerian background mesh.
     */
    void applySeismicVelocityMarkedSTLNodes(double currentTime, double dt, Mesh* mesh);
    
    /**
     * @brief Updates the accumulated seismic velocity and acceleration based on the current time and time step.
     * 
     * @param currentTime Current simulation time (s).
     * @param dt Simulation time step (s).
     * @note This function updates the global `accumulatedVelocity` variable with the seismic velocity
     */
    void updateSeismicVectors(const double currentTime, const double dt);

    /**
     * @brief Marks the nodes that will receive seismic loading.
     * @param epsilon Grid dimension factor threshold to mark nodes as seismic nodes.
     * @param mesh Pointer to the mesh where nodes are defined.
     */
    void markSeismicNodes(double epsilon, Mesh* mesh);

    /**
     * @brief Returns the list of seismic node indices.
     * @return const std::vector<int>& Reference to the indices of seismic nodes.
     */
    const std::vector<int>& getSeismicNodeIndices();
    
    /**
     * @brief Checks if a node is marked as seismic.
     * @param nodeId Node index to check.
     * @return true if the node is a seismic node, false otherwise.
     */
    bool isSeismicNode(int nodeId);

    /**
     * @brief Returns the accumulated seismic velocity.
     * @return Eigen::Vector3d& Reference to the accumulated velocity vector.
     */
    const Eigen::Vector3d& getAccumulatedVelocity();

    /**
     * @brief Returns the seismic acceleration.
     * @return Eigen::Vector3d& Reference to the seismic acceleration vector.
     */
    const Eigen::Vector3d& getAcceleration();

    /**
     * @brief Disables seismic analysis.
     *
     * This function sets the seismic analysis to inactive and clears the seismic data.
     */
    void disableSeismicAnalysis();
}