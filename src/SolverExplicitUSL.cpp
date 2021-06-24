/*
 * SolverExplicitUSL.cpp
 *
 *  Created on: 13 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <vector>
using std::vector;

#include <iostream>
using std::cout;

#include "SolverExplicitUSL.h"
#include "Model.h"
#include "Integration.h"
#include "Interpolation.h"
#include "Update.h"
#include "Output.h"

SolverExplicitUSL::SolverExplicitUSL():Solver() {

}

SolverExplicitUSL::~SolverExplicitUSL() {
	
}

void SolverExplicitUSL::Solve( ){

	// simulation variables
	double time=ModelSetup::getTime();
	double dt=ModelSetup::getTimeStep();
	int resultSteps=ModelSetup::getResultSteps();
	double iTime=0.0;
	int loopCounter=0;

	// solve in time
	while(iTime<time) {

		// write results
		if (loopCounter++%resultSteps==0){

			Output::writeBodies(bodies,iTime);
			cout<<"Time = "<<iTime<<"\n";
		}

		// update contribution nodes
		Update::contributionNodes(mesh,bodies);
		
		// nodal mass
		Interpolation::nodalMass(mesh,bodies);

		// nodal momentum
		Interpolation::nodalMomentum(mesh,bodies);
		
		// impose essential boundary condition on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		// nodal internal force
		Interpolation::nodalInternalForce(mesh,bodies);
		
		// nodal external force
		Interpolation::nodalExternalForce(mesh,bodies);

		// nodal total force
		Update::nodalTotalForce(mesh);

		// impose force boundary conditions
		Update::boundaryConditionsForce(mesh);

		// integrate the grid nodal momentum equation
		Integration::nodalMomentum(mesh,loopCounter==1?dt/2.0:dt);

		// update particle velocity
		Update::particleVelocity(mesh,bodies,loopCounter==1?dt/2.0:dt);
		
		// update particle position
		Update::particlePosition(mesh,bodies,dt);

		// nodal velocity
		Update::nodalVelocity(mesh);

		// calculate particle strain increment
		Interpolation::particleStrainIncrement(mesh,bodies,dt);
		
		// calculate particle vorticity increment
		Interpolation::particleVorticityIncrement(mesh,bodies,dt);
		
		// update particle density
		Update::particleDensity(bodies);
		
		// update particle stress
		Update::particleStress(bodies);
		
		// reset all nodal values
		Update::resetNodalValues(mesh);

		// advance in time
		iTime+=dt;
	}

	// write the Eulerian mesh
	Output::writeGrid(mesh,Output::CELLS);

	// write results series
	Output::writeResultsSeries();
}
