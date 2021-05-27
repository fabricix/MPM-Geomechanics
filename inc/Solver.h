/*
 * Solver.h
 *
 *  Created on: 13 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <vector>
using std::vector;

class Body;
class Mesh;

/// \class Solver
/// \brief Represents the operations to solve the equations in time.
class Solver {

public:

	/// \brief Default constructor
	///
	Solver();

	/// \brief Default destructor
	///
	virtual ~Solver();
	
	/// \brief Solve the problem in time
	///
	virtual void Solve(vector<Body*>& bodies, Mesh& mesh)=0;
};

#endif /* SOLVER_H_ */
