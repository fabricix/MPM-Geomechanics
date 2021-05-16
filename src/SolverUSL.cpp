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

SolverUSL::SolverUSL() {
	// TODO Auto-generated constructor stub

}

SolverUSL::~SolverUSL() {
	// TODO Auto-generated destructor stub
}

void SolverUSL::Solve(vector<Body>& bodies, Mesh& mesh, Update& update, Output& output, Interpolation& interpola, Integration& integra){

	double time=ModelSetup::getTime();
	double dt=ModelSetup::getDt();
	int resultSteps=ModelSetup::getResultSteps();
	double iTime=0.0;
	int loopCounter=1;

	// solve in time
	while(iTime<time){

		for (size_t i = 0; i < bodies.size(); ++i){

			// update contribution nodes
			update.contributionNodes(mesh,bodies.at(i).getParticles());
			
			// nodal mass
			interpola.nodalMass(mesh,bodies.at(i).getParticles());

			// nodal momentum
			interpola.nodalMomentum(mesh,bodies.at(i).getParticles());
		}

		// impose essential boundary condition on nodal momentum
		update.boundaryConditionsMomentum(mesh);

		for (size_t i = 0; i < bodies.size(); ++i){

			// nodal internal force
			interpola.nodalInternalForce(mesh,bodies.at(i).getParticles());
			
			// nodal external force
			interpola.nodalExternalForce(mesh,bodies.at(i).getParticles());
		}

		// nodal total force
		update.nodalTotalForce(mesh);

		// impose force boundary conditions
		update.boundaryConditionsForce(mesh);

		// integrate the grid nodal momentum equation
		integra.nodalMomentum(mesh,loopCounter==1?dt/2.0:dt);

		for (size_t i = 0; i < bodies.size(); ++i){

			// update particle velocity
			update.particleVelocity(mesh,bodies.at(i).getParticles(),loopCounter==1?dt/2.0:dt);
			
			// update particle position
			update.particlePosition(mesh,bodies.at(i).getParticles(),dt);
		}

		// nodal velocity
		update.nodalVelocity(mesh);

		for (size_t i = 0; i < bodies.size(); ++i){

			// calculate particle strain increment
			interpola.particleStrainIncrement(mesh,bodies.at(i).getParticles(),dt);
			
			// calculate particle vorticity increment
			interpola.particleVorticityIncrement(mesh,bodies.at(i).getParticles(),dt);
			
			// update particle density
			update.particleDensity(bodies.at(i).getParticles());
			
			// update particle stress
			update.particleStress(bodies.at(i).getParticles());
		}
		
		// reset all nodal values
		update.resetNodalValues(mesh);

		// write results
		if (loopCounter++%resultSteps==0)
		{
			for (size_t i = 0; i < bodies.size(); ++i){

				output.writeParticles(bodies.at(i).getParticles(),time);
			}
		}

		// advance in time
		iTime+=dt;
	}
}
