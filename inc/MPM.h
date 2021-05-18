/*
 * MPM.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MPM_H_
#define MPM_H_

#include "Mesh.h"
#include "Body.h"
#include "Model.h"
#include "Output.h"
#include "Input.h"
#include "States.h"
#include "Solver.h"

/// \class MPM
/// \brief Components and algorithms of the MPM
/// This class contain the basic objects of a MPM model,
/// and the operations between them.
class MPM {

public:

	MPM(); //!< default constructor
	virtual ~MPM(); //!< default destructor

	bool readInputFile(int argc, char **argv); //!< read the input file of the model
	void createModel(); //!< create the numerical MPM model
	void solve(); //!< solve the problem in time
	void end(); //!< finish all program structure

private:
	
	Mesh mesh; //!< grid mesh
	vector<Body*> bodies; //!< bodies discretized by material points
	vector<Material*> materials; //!< bodies discretized by material points
	States states; //!< operations for load and save model's states
	Solver* solver; //!< operation for solve the equations in time
	Input input; //!< input operations

	bool setSimulationTime(); //!< configures the simulation time
	bool setSolver(); //!< configures the solver of the model
	bool setTimeStep(); //!< configures the time step 
	bool setUpMesh(); //!< configures the mesh
	void setUpMaterialList(); //!< configures the material list
	void setUpBodyList(); //!< configures the body list
	void createBodies(); //!< configures the body list
	bool setInterpolationFunctions(); //!< configures the shape function
	void setUpParticles(); //!< configures the particles
};

#endif /* MPM_H_ */
