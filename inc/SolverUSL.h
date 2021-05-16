/*
 * SolverUSL.h
 *
 *  Created on: 13 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SOLVERUSL_H_
#define SOLVERUSL_H_

#include "Solver.h"
#include "Update.h"
#include "Body.h"
#include "Mesh.h"
#include "Output.h"
#include "Interpolation.h"
#include "Integration.h"

/// \class SolverUSL
/// \brief Represents the operations to solve the equations in time using the scheme Update Stress Last
class SolverUSL: public Solver {

public:

	SolverUSL(); //!< default constructor
	virtual ~SolverUSL(); //!< default destructor

	virtual void Solve(vector<Body>&, Mesh&, Update&, Output&, Interpolation&, Integration&);
};

#endif /* SOLVERUSL_H_ */
