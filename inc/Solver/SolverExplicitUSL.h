/*
 * SolverExplicitUSL.h
 *
 *  Created on: 13 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SOLVEREXPLICITUSL_H_
#define SOLVEREXPLICITUSL_H_

#include "Solver.h"
#include "Body/Body.h"
#include "Mesh/Mesh.h"

/// \class SolverExplicitUSL
/// \brief Represents the operations to solve explicitly the
/// equations in time using the scheme Update Stress Last
class SolverExplicitUSL: public Solver {

public:

	/// \brief Default constructor
	///
	SolverExplicitUSL();
	
	/// \brief Default destructor
	///
	virtual ~SolverExplicitUSL();
	
	/// \brief Solve the problem in time
	///
	virtual void Solve( ); 
};

#endif /* SOLVEREXPLICITUSL_H_ */
