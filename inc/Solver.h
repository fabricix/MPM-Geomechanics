/*
 * Solver.h
 *
 *  Created on: 13 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SOLVER_H_
#define SOLVER_H_

/// \class Solver
/// \brief Represents the operations to solve the equations in time.
class Solver {

public:

	Solver(); //!< default constructor
	virtual ~Solver(); //!< default destructor
	
	virtual void Solve()=0; //!< pure virtual method
};

#endif /* SOLVER_H_ */
