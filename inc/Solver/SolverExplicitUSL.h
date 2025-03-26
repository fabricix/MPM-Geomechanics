// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef SOLVEREXPLICITUSL_H_
#define SOLVEREXPLICITUSL_H_

#include "Solver.h"

/// \class SolverExplicitUSL
/// \brief Represents the operations to solve explicitly the
/// equations in time using the scheme Update Stress Last
class SolverExplicitUSL: public Solver {

public:

	/// \brief Default constructor
	///
	SolverExplicitUSL();
	
	/// \brief Solve the problem in time
	///
	virtual void Solve( ); 
};

#endif /* SOLVEREXPLICITUSL_H_ */
