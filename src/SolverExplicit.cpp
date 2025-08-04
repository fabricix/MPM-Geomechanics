// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Solver/SolverExplicit.h"
#include "Model.h"
#include "Integration.h"
#include "Interpolation.h"
#include "Update.h"
#include "Output.h"
#include "DynamicRelaxation.h"
#include "Energy.h"
#include "TerrainContact.h"
#include "Seismic.h"

SolverExplicit::SolverExplicit() : Solver() {}

void SolverExplicit::Solve()
{
	// Initialization of simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	int loopCounter = 0;
	bool useMUSL = (ModelSetup::getUpdateStressScheme() == ModelSetup::MUSL);
	bool useSTLContact = ModelSetup::getTerrainContactActive();
	bool isSeismicAnalysis = ModelSetup::getSeismicAnalysisActive();

	// Time integration loop
	while (iTime < time)
	{
		// Write output results
		Output::writeResultInStep(loopCounter++, resultSteps, bodies, iTime);

		// Step 1: Particle-to-Grid mass and momentum interpolation
		Update::contributionNodes(mesh, bodies);
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			Interpolation::nodalMass(mesh, bodies);

			#pragma omp section
			Interpolation::nodalMomentum(mesh, bodies);
		}

		// Update seismic velocity and acceleration from record
		if(isSeismicAnalysis){
			Seismic::updateSeismicVectors(iTime, loopCounter == 1 ? dt / 2.0 : dt);
		}

		// Step 2: Impose boundary conditions on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		// Step 3: Compute nodal forces
		// 3.1: Internal and external nodal force
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			Interpolation::nodalInternalForce(mesh, bodies);

			#pragma omp section
			Interpolation::nodalExternalForce(mesh, bodies);
		}

		// 3.2: Total force nodal force
		Update::nodalTotalForce(mesh);

		// 3.3: Impose boundary conditions on total force
		Update::boundaryConditionsForce(mesh);

		// Step 4: Integrate nodal momentum
		Integration::nodalMomentum(mesh, loopCounter == 1 ? dt / 2.0 : dt);

		// Step 5: Particle updates
		// 5.1: Update particle velocity
		Update::particleVelocity(mesh, bodies, loopCounter == 1 ? dt / 2.0 : dt);

		// 5.2: Apply contact correction in particle velocity
		if (useSTLContact){

			// 5.2.1: Apply seismic velocity to marked nodes
			if (isSeismicAnalysis){
				Seismic::applySeismicVelocityMarkedSTLNodes(mesh);
			}
			terrainContact->apply(mesh, particles, dt);
		}

		// 5.3: Update particle position
		Update::particlePosition(mesh, bodies, dt);

		// Step 6 (MUSL): Momentum recalculation 
		if (useMUSL)
		{	
			// 6.1: Recalculate nodal momentum
			Update::resetNodalMomentum(mesh);
			Interpolation::nodalMomentum(mesh, bodies);
			
			// 6.2: Reapply BCs on nodal momentum
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

		// Compute current kinetic energy
		Energy::computeKineticEnergy(bodies);

		// Compute current kinetic energy
		Energy::computeKineticEnergy(bodies);

		// Static solution check (Dynamic Relaxation)
		DynamicRelaxation::setStaticSolution(bodies, loopCounter);

		// Step 11: Advance simulation time
		ModelSetup::setCurrentTime(iTime += dt);
	}

	// Final output
	Output::writeGrid(mesh, Output::CELLS);
	Output::writeResultsSeries();
}
