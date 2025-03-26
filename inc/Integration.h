// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INTEGRATION_H_
#define INTEGRATION_H_

#include "Mesh/Mesh.h"

/// \namespace Integration
/// \brief Represents the integration operations in the MPM.
namespace Integration {

	/// \brief Integrate the nodal momentum
	/// \param[in] mesh Mesh reference
	/// \param[in] dt Time step
	void nodalMomentum(Mesh* mesh, double dt);
};

#endif /* INTEGRATION_H_ */
