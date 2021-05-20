/*
 * SolverUSL.cpp
 *
 *  Created on: 13 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <vector>
using std::vector;

#include "SolverUSL.h"
#include "Model.h"
#include "Integration.h"
#include "Interpolation.h"
#include "Update.h"
#include "Output.h"

SolverUSL::SolverUSL():Solver() {
	// TODO Auto-generated constructor stub

}

SolverUSL::~SolverUSL() {
	// TODO Auto-generated destructor stub
}

void SolverUSL::Solve(vector<Body*>& bodies, Mesh& mesh){

	double time=ModelSetup::getTime();
	double dt=ModelSetup::getTimeStep();
	int resultSteps=ModelSetup::getResultSteps();
	double iTime=0.0;
	int loopCounter=1;

	// solve in time
	while(iTime<time){

		for (size_t i = 0; i < bodies.size(); ++i){

			// update contribution nodes
			Update::contributionNodes(mesh,bodies.at(i)->getParticles());
			
			// nodal mass
			Interpolation::nodalMass(mesh,bodies.at(i)->getParticles());

			// nodal momentum
			Interpolation::nodalMomentum(mesh,bodies.at(i)->getParticles());
		}

		// impose essential boundary condition on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		for (size_t i = 0; i < bodies.size(); ++i){

			// nodal internal force
			Interpolation::nodalInternalForce(mesh,bodies.at(i)->getParticles());
			
			// nodal external force
			Interpolation::nodalExternalForce(mesh,bodies.at(i)->getParticles());
		}

		// nodal total force
		Update::nodalTotalForce(mesh);

		// impose force boundary conditions
		Update::boundaryConditionsForce(mesh);

		// integrate the grid nodal momentum equation
		Integration::nodalMomentum(mesh,loopCounter==1?dt/2.0:dt);

		for (size_t i = 0; i < bodies.size(); ++i){

			// update particle velocity
			Update::particleVelocity(mesh,bodies.at(i)->getParticles(),loopCounter==1?dt/2.0:dt);
			
			// update particle position
			Update::particlePosition(mesh,bodies.at(i)->getParticles(),dt);
		}

		// nodal velocity
		Update::nodalVelocity(mesh);

		for (size_t i = 0; i < bodies.size(); ++i){

			// calculate particle strain increment
			Interpolation::particleStrainIncrement(mesh,bodies.at(i)->getParticles(),dt);
			
			// calculate particle vorticity increment
			Interpolation::particleVorticityIncrement(mesh,bodies.at(i)->getParticles(),dt);
			
			// update particle density
			Update::particleDensity(bodies.at(i)->getParticles());
			
			// update particle stress
			Update::particleStress(bodies.at(i)->getParticles());
		}
		
		// reset all nodal values
		Update::resetNodalValues(mesh);

		// write results
		if (loopCounter++%resultSteps==0)
		{
			for (size_t i = 0; i < bodies.size(); ++i){

				Output::writeBodies(bodies,iTime);
			}
		}

		// advance in time
		iTime+=dt;
	}

	// write the Eulerian mesh
	Output::writeGrid(mesh,Output::POINTS);

	// write results series
	Output::writeResultsSeries();
}
