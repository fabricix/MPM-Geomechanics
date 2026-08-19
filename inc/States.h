// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <string>
#include "Particle/Particle.h"

#ifndef STATES_H_
#define STATES_H_

/// \namespace States
/// \brief Functions to save and load states of the simulation
namespace States {

	/// \brief Save the particles stress into a json file
	/// \param[in] filename File name
	/// \param[in] particles List containing pointers to particles
	void saveParticleStress(const std::string& filename, const std::vector<Particle*>& particles);

	/// \brief Load the particles stress from a json file
	/// \param[in] filename File name
	/// \param[in] particles List containing pointers to particles
	void loadParticleStress(const std::string& filename, std::vector<Particle*>& particles);
}

#endif /* STATES_H_ */