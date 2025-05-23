// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <vector>
using std::vector;

#include <iostream>
using std::cout;

#include "Solver/SolverExplicitUSL.h"
#include "Model.h"
#include "Integration.h"
#include "Interpolation.h"
#include "Update.h"
#include "Output.h"
#include "DynamicRelaxation.h"
#include "TerrainContact.h"

SolverExplicitUSL::SolverExplicitUSL() : Solver() {}

void SolverExplicitUSL::Solve()
{
	// initialize simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	int loopCounter = 0;

	// solve in time
	while (iTime < time)
	{
		// write results in the result step
		Output::writeResultInStep(loopCounter++, resultSteps, bodies, iTime);

		// update contribution nodes
		Update::contributionNodes(mesh, bodies);

		#pragma omp parallel sections num_threads(2)
		{
			// nodal mass
			#pragma omp section
			Interpolation::nodalMass(mesh, bodies);

			// nodal momentum
			#pragma omp section
			Interpolation::nodalMomentum(mesh, bodies);
		}

		// impose essential boundary condition on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		#pragma omp parallel sections num_threads(2)
		{
			// nodal internal force
			#pragma omp section
			Interpolation::nodalInternalForce(mesh, bodies);

			// nodal external force
			#pragma omp section
			Interpolation::nodalExternalForce(mesh, bodies);
		}

		// nodal total force
		Update::nodalTotalForce(mesh);

		// impose force boundary conditions
		Update::boundaryConditionsForce(mesh);

		// integrate the grid nodal momentum equation
		Integration::nodalMomentum(mesh, loopCounter == 1 ? dt / 2.0 : dt);

		// update particle velocity
		Update::particleVelocity(mesh, bodies, loopCounter == 1 ? dt / 2.0 : dt);

		// contact treatment
		if (ModelSetup::getTerrainContactActive())
		{
			// calculate the distance level set function to particles
			terrainContact->particleDistanceLevelSet(mesh, particles);

			// calculate the nodal density level-set
			terrainContact->nodalDensityLevelSet(mesh, particles);

			// interpolate the density at the center of triangles
			terrainContact->trianglesDensityLevelSet(mesh);

			// determine the contact potential pairs
			terrainContact->determineContactPotentialPairs(mesh, particles);

			// compute the contact forces and correct velocities
			terrainContact->computeContactForces(particles, dt);
		}
		
		// update particle position
		Update::particlePosition(mesh, bodies, dt);

		// nodal velocity
		Update::nodalVelocity(mesh);

		#pragma omp parallel sections num_threads(2)
		{
			// calculate particle strain increment
			#pragma omp section
			Interpolation::particleStrainIncrement(mesh, bodies, dt);

			// calculate particle vorticity increment
			#pragma omp section
			Interpolation::particleVorticityIncrement(mesh, bodies, dt);
		}

		// update particle density
		Update::particleDensity(bodies);

		// update particle stress
		Update::particleStress(bodies);

		// reset all nodal values
		Update::resetNodalValues(mesh);

		// verify the static solution requirements
		DynamicRelaxation::setStaticSolution(bodies, loopCounter);

		// advance in time
		ModelSetup::setCurrentTime(iTime += dt);
	}

	// write the Eulerian mesh
	Output::writeGrid(mesh, Output::CELLS);

	// write results series
	Output::writeResultsSeries();
}