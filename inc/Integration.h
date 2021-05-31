/*
 * Integration.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INTEGRATION_H_
#define INTEGRATION_H_

#include "Mesh.h"

/// \namespace Integration
/// \brief Represents the integration operations in the MPM.
namespace Integration {

	/// \brief Integrate the nodal momentum
	/// \param[in] mesh Mesh reference
	/// \param[in] dt Time step
	void nodalMomentum(Mesh& mesh, double dt);
};

#endif /* INTEGRATION_H_ */
