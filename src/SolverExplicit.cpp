// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Solver/SolverExplicit.h"
#include "Model.h"
#include "Integration.h"
#include "Interpolation.h"
#include "Update.h"
#include "Output.h"
#include "DynamicRelaxation.h"
#include "TerrainContact.h"

SolverExplicit::SolverExplicit() : Solver() {}

void SolverExplicit::Solve()
{
	// Initialize simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	int loopCounter = 0;
	bool useMUSL = (ModelSetup::getUpdateStressScheme() == ModelSetup::MUSL);
	bool useSTLContact = ModelSetup::getTerrainContactActive();
	bool isSeismicAnalysis = ModelSetup::getSeismicAnalysis();

	// Solve in time
	while (iTime < time)
	{
		// Output results
		Output::writeResultInStep(loopCounter++, resultSteps, bodies, iTime);

		// Step 1: Interpolate mass and momentum from Particles to Nodes
		Update::contributionNodes(mesh, bodies);
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			Interpolation::nodalMass(mesh, bodies);

			#pragma omp section
			Interpolation::nodalMomentum(mesh, bodies);
		}

		// Step 2: Apply boundary conditions on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		// Step 3.1: Interpolate internal and external force from particles to nodes
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			Interpolation::nodalInternalForce(mesh, bodies);

			#pragma omp section
			Interpolation::nodalExternalForce(mesh, bodies);
		}

		// Step 3.2: Compute total nodal force
		Update::nodalTotalForce(mesh);

		// Step 3.2.1: Apply seismic loading (if active)
    	if (useSTLContact && isSeismicAnalysis) {
        	terrainContact->applySeismicForce(iTime);
    	}
		
		// Step 3.3: Apply boundary conditions on total force
		Update::boundaryConditionsForce(mesh);

		// Step 4: Integrate nodal momentum
		Integration::nodalMomentum(mesh, loopCounter == 1 ? dt / 2.0 : dt);

		// Step 5.1: Update particle velocity
		Update::particleVelocity(mesh, bodies, loopCounter == 1 ? dt / 2.0 : dt);

		// Step 5.2: Apply contact correction in particle velocity (if active)
		if (useSTLContact){
			terrainContact->apply(mesh, particles, dt);
		}

		// Step 5.3: Update particle position
		Update::particlePosition(mesh, bodies, dt);

		// Step 6 (MUSL only): Recalculate nodal momentum and apply BCs on nodal momentum 
		if (useMUSL)
		{	
			// Step 6.1 (MUSL only): Recalculate nodal momentum with updated particle velocity
			Update::resetNodalMomentum(mesh);
			Interpolation::nodalMomentum(mesh, bodies);
			
			// Step 6.2 (MUSL only): Reapply BCs on nodal momentum
			Update::boundaryConditionsMomentum(mesh);
		}

		// Step 7: Compute nodal velocity
		Update::nodalVelocity(mesh);

		// Step 8: Update strain and vorticity increments
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			Interpolation::particleStrainIncrement(mesh, bodies, dt);

			#pragma omp section
			Interpolation::particleVorticityIncrement(mesh, bodies, dt);
		}

		// Step 9: Update density and stress
		Update::particleDensity(bodies);
		Update::particleStress(bodies);

		// Step 10: Reset nodal values
		Update::resetNodalValues(mesh);

		// Check for static solution
		DynamicRelaxation::setStaticSolution(bodies, loopCounter);

		// Advance time
		ModelSetup::setCurrentTime(iTime += dt);
	}

	// Write results
	Output::writeGrid(mesh, Output::CELLS);
	Output::writeResultsSeries();
}
