// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once

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

struct SeismicAnalysis {
	bool isActive = false;
	std::string filename = "";
	bool hasHeader = true;
};

namespace Seismic
{
    void setSeismicData(); // set acceleration data from file
    SeismicData& getSeismicData(); // get seismic acceleration data

    SeismicAnalysis& getSeismicAnalysis();
    void setSeismicAnalysis(const SeismicAnalysis& info);
}