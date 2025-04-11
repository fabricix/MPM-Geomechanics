// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef SOLVEREXPLICITTWOPHASEUSL_H_
#define SOLVEREXPLICITTWOPHASEUSL_H_

#include <Solver/Solver.h>

/// \class SolverExplicitTwoPhaseUSL
/// \brief Solves explicitly the two phase motion equation in time
/// using the scheme Update Stress Last
class SolverExplicitTwoPhaseUSL: public Solver {

public:

    /// \brief Default constructor
    ///
	SolverExplicitTwoPhaseUSL();
    
    /// \brief Solve the problem in time
    ///
    virtual void Solve( ); 
};

#endif /* SOLVEREXPLICITTWOPHASEUSL_H_ */
