// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once

#include "Particle/Particle.h"
#include <vector>
#include <string>
#include <Eigen/Core>

namespace HydroMechanicalCoupling 
{
	/// Check if one-way hydro-mechanical coupling is active
	bool isOneWayEnabled();

	/// Assign pore pressure to particles based on position
	void configureOneDirectionCoupling(std::vector<Particle*>& particles);

	/// Compute total stress tensor: sigma_total = sigma_effective + p * I
	Eigen::Matrix3d computeTotalStress(const Eigen::Matrix3d& sigma_effective, double pore_pressure);
}
