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
	virtual void Solve()=0;

	/// \brief Register the mesh in the solver
	///
	inline void registerMesh(Mesh* mesh){ this->mesh=mesh; }
	
	/// \brief Register the bodies in the solver
	///
	inline void registerBodies(vector<Body*>* bodies){ this->bodies=bodies; }

protected:

	Mesh* mesh; //!< pointer to mesh
	vector<Body*>* bodies; //!< pointer to bodies
};

inline Solver::Solver():mesh(0),bodies(0) {
	
}

inline Solver::~Solver() {
	
}

#endif /* SOLVER_H_ */
