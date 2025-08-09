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
#include "Parallelization.h"
#include <iostream>

SolverExplicit::SolverExplicit() : Solver() {}

void SolverExplicit::Solve()
{
	// Initialization of simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int numThreads = ModelSetup::getThreads();
	int factor = ModelSetup::getPartitionFactor();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	bool useMUSL = (ModelSetup::getUpdateStressScheme() == ModelSetup::MUSL);
	bool useSTLContact = ModelSetup::getTerrainContactActive();
	bool isSeismicAnalysis = ModelSetup::getSeismicAnalysisActive();

	// write initial particles and grid states
	Output::writeInitialState(resultSteps, bodies, iTime, mesh);

	// Time integration loop
	while (iTime < time)
	{
		// increment loop counter
		ModelSetup::incrementLoopCounter();

		// update contribution nodes
		Update::contributionNodes(mesh, particles);

		// calculate the interfaces particles
		Parallelization::calculateInterfaceParticles(mesh, *particlesPerThread);

		// Step 1: Interpolate mass and momentum from Particles to Nodes
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			//Parallelization::interpolateMass(mesh, *particlesPerThread, factor);

			Interpolation::nodalMass(mesh, particles);

			#pragma omp section
			Interpolation::nodalMomentum(mesh, particles);
		}

		// Update seismic velocity and acceleration from record
		if(isSeismicAnalysis){
			Seismic::updateSeismicVectors(iTime, ModelSetup::getLoopCounter() == 1 ? dt / 2.0 : dt);
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
		Integration::nodalMomentum(mesh, ModelSetup::getLoopCounter() == 1 ? dt / 2.0 : dt);

		// Step 5.1: Update particle velocity
		Update::particleVelocity(mesh, bodies, ModelSetup::getLoopCounter() == 1 ? dt / 2.0 : dt);

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
			Interpolation::nodalMomentum(mesh, particles);
			
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
		
		// write particles and grid in step
		Output::writeResultInStep(resultSteps, bodies, iTime);
		Output::writeGridInStep(resultSteps, mesh, iTime);

		// Step 10: Reset nodal values
		Update::resetNodalValues(mesh);

		// Compute current kinetic energy
		Energy::computeKineticEnergy(particles);

		// Static solution check (Dynamic Relaxation)
		DynamicRelaxation::setStaticSolution(particles);

		// Step 11: Advance simulation time
		ModelSetup::setCurrentTime(iTime += dt);
	}

	Output::writeResultsSeries();
}
