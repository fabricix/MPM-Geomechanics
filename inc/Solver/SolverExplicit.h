// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef SOLVEREXPLICIT_H_
#define SOLVEREXPLICIT_H_

#include "Solver.h"

/// \class SolverExplicit
/// \brief Represents the operations to solve explicitly the
/// equations in time using the Update Stress Last (USL) or Modified Update Stress Last (MUSL) schemes.
///
/// This solver can operate in either of two integration modes:
/// - USL (Update Stress Last): classical explicit integration where stress is updated after particle velocity and position.
/// - MUSL (Modified Update Stress Last): improves accuracy by updating particle stress using the recalculated nodal grid momentum from updated particle momentum.
///
/// \par Pseudocode for both schemes:
///
/// \b USL and MUSL:
/// \code
/// 1. Interpolate mass and momentum from Particles to nodes
/// 2. Apply boundary conditions on nodal momentum (p=0 for fixed nodes)
/// 3.1 Interpolate internal and external force from particles to nodes
/// 3.2 Compute total nodal force
/// 3.3 Apply boundary conditions on total nodal force (f=0 for fixed nodes)
/// 4. Solve motion equation: update nodal momentum
/// 6.1 If MUSL: Recalculate nodal momentum with the updated particle velocity
/// 6.2 If MUSL: Apply boundary conditions on nodal momentum (p=0 for fixed nodes)
/// 5.1 Update particle velocity using nodal acceleration
/// 5.2 Apply contact correction on particle velocity (if active)
/// 5.3 Update particle position using updated nodal momentum
/// 7. Update nodal velocity with the updated nodal momentum
/// 8. Update particle strain increment and vorticity increment using updated nodal velocity
/// 9. Update particle density and stress strain and vorticity increment
/// 10. Reset nodal variables
/// \endcode
///
class SolverExplicit : public Solver {

public:

	/// \brief Default constructor
	SolverExplicit();

	/// \brief Solve the problem in time using either USL or MUSL
	virtual void Solve();
};

#endif /* SOLVEREXPLICIT_H_ */
