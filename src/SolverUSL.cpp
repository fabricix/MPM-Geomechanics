/*
 * SolverUSL.cpp
 *
 *  Created on: 13 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <vector>
using std::vector;

#include <iostream>
using std::cout;

#include "SolverUSL.h"
#include "Model.h"
#include "Integration.h"
#include "Interpolation.h"
#include "Update.h"
#include "Output.h"

SolverUSL::SolverUSL():Solver() {

}

SolverUSL::~SolverUSL() {
	
}

void SolverUSL::Solve(vector<Body*>& bodies, Mesh& mesh){

	// simulation variables
	double time=ModelSetup::getTime();
	double dt=ModelSetup::getTimeStep();
	int resultSteps=ModelSetup::getResultSteps();
	double iTime=0.0;
	int loopCounter=1;

	// write the initial state
	for (size_t i = 0; i < bodies.size(); ++i){

		Output::writeBodies(bodies,iTime);
	}

	// solve in time
	while(iTime<time){

		for (size_t i = 0; i < bodies.size(); ++i){

			// particles
			vector<Particle*>* particles=bodies.at(i)->getParticles();

			// update contribution nodes
			Update::contributionNodes(mesh,particles);
			
			// nodal mass
			Interpolation::nodalMass(mesh,particles);

			// nodal momentum
			Interpolation::nodalMomentum(mesh,particles);
		}

		// impose essential boundary condition on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		for (size_t i = 0; i < bodies.size(); ++i){

			// particles
			vector<Particle*>* particles=bodies.at(i)->getParticles();

			// nodal internal force
			Interpolation::nodalInternalForce(mesh,particles);
			
			// nodal external force
			Interpolation::nodalExternalForce(mesh,particles);
		}

		// nodal total force
		Update::nodalTotalForce(mesh);

		// impose force boundary conditions
		Update::boundaryConditionsForce(mesh);

		// integrate the grid nodal momentum equation
		Integration::nodalMomentum(mesh,loopCounter==1?dt/2.0:dt);

		for (size_t i = 0; i < bodies.size(); ++i){

			// particles
			vector<Particle*>* particles=bodies.at(i)->getParticles();

			// update particle velocity
			Update::particleVelocity(mesh,particles,loopCounter==1?dt/2.0:dt);
			
			// update particle position
			Update::particlePosition(mesh,particles,dt);
		}

		// nodal velocity
		Update::nodalVelocity(mesh);

		for (size_t i = 0; i < bodies.size(); ++i){

			// particles
			vector<Particle*>* particles=bodies.at(i)->getParticles();

			// calculate particle strain increment
			Interpolation::particleStrainIncrement(mesh,particles,dt);
			
			// calculate particle vorticity increment
			Interpolation::particleVorticityIncrement(mesh,particles,dt);
			
			// update particle density
			Update::particleDensity(particles);
			
			// update particle stress
			Update::particleStress(particles);
		}
		
		// reset all nodal values
		Update::resetNodalValues(mesh);

		// write results
		if (loopCounter++%resultSteps==0)
		{
			for (size_t i = 0; i < bodies.size(); ++i){

				Output::writeBodies(bodies,iTime);
        		std::cout<<"Time = "<<iTime<<"\n";
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
