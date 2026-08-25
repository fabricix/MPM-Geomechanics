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
#include "ContactManager.h"

SolverExplicit::SolverExplicit() : Solver() {}

void SolverExplicit::Solve()
{
	// Initialization of simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	bool useMUSL = (ModelSetup::getUpdateStressScheme() == ModelSetup::MUSL);
	bool useSTLContact = ModelSetup::getTerrainContactActive();
	bool contactActive = ModelSetup::getContactActive();
	int loopCounter = 0;
	ModelSetup::setLoopCounter(loopCounter);

	// write initial particles and grid states
	Output::writeInitialState(bodies, iTime, mesh, terrainContact);

	// Time integration loop
	while (iTime < time)
	{
		// increment loop counter
		loopCounter = ModelSetup::incrementLoopCounter();

		// Step 1: Particle-to-Grid mass and momentum interpolation
		Update::contributionNodes(mesh, particles);

		// First contact detection
		if (contactActive) {
			contactManager->contactNodalDetection(mesh, bodies);
		}

		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			Interpolation::nodalMass(mesh, particles);
			
			#pragma omp section
			Interpolation::nodalMomentum(mesh, particles);
		}
		
		// Compute nodal unit normal vector for contact forces calculation
		if (contactActive) {
			contactManager->nodalUnitNormal(mesh, bodies);
		}

		// Step 1.c: Update seismic velocity and acceleration from record
		Seismic::updateSeismicVectors(iTime, loopCounter == 1 ? dt / 2.0 : dt);

		// Step 2: Impose boundary conditions on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		// Step 3.1: Internal nodal force
		Interpolation::nodalInternalForce(mesh, particles);
		
		// Step 3.2: External nodal force
		Interpolation::nodalExternalForce(mesh, particles);

		// Step 3.3: Total force nodal force
		Update::nodalTotalForce(mesh);

		// Step 3.3: Impose boundary conditions on total force
		Update::boundaryConditionsForce(mesh);

		// Step 4: Integrate nodal momentum
		Integration::nodalMomentum(mesh, loopCounter == 1 ? dt / 2.0 : dt);

		// Apply multi field velocity contact correction
		if (contactActive) {
			contactManager->applyVelocityContactCorrection(mesh,bodies,dt);
		}

		// Step 5: Particle updates
		// 5.1: Update particle velocity
		Update::particleVelocity(mesh, particles, loopCounter == 1 ? dt / 2.0 : dt);

		// Step 5.2: Apply contact correction in particle velocity
		if (useSTLContact){

			// 5.2.a: Apply seismic velocity to marked nodes
			Seismic::applySeismicVelocityMarkedSTLNodes(mesh);
			
			// 5.2.b: Apply velocity contact correction
			terrainContact->apply(mesh, particles, dt);
		}

		// Step 5.3: Update particle position
		Update::particlePosition(mesh, particles, dt);
 
		// Step 5.4: Project particle position if any penetrations
		if (useSTLContact) terrainContact->projectParticles(mesh, particles);

		// Step 6 (MUSL): Momentum recalculation 
		if (useMUSL)
		{	
			// Step 6.1: Recalculate nodal momentum
			Update::resetNodalMomentum(mesh);
			if (contactActive) {
				contactManager->nodalMomentumContactUpdate(mesh, dt);
			}

			Interpolation::nodalMomentum(mesh, particles);
			
			// Step 6.2: Reapply BCs on nodal momentum
			Update::boundaryConditionsMomentum(mesh);
		}

		// Step 7: Compute nodal velocity
		Update::nodalVelocity(mesh);

		// Step 8.1: Update strain increments
		Interpolation::particleStrainIncrement(mesh, particles, dt);

		// Step 8.2: Update vorticity increments
		Interpolation::particleVorticityIncrement(mesh, particles, dt);

		// Step 9.1: Update density 
		Update::particleDensity(particles);

		// Step 9.2: Update stress
		Update::particleStress(particles);
		
		// write particles and grid results in step
		Output::writeResultInStep(resultSteps, particles, iTime);
		Output::writeGridInStep(resultSteps, mesh, iTime);
		Output::writeSTLContactInStep(resultSteps, terrainContact, iTime);

		// Step 10: Reset nodal values
		Update::resetNodalValues(mesh);

		// Step 11: Compute current kinetic energy
		Energy::computeKineticEnergy(particles);

		// Step 12: Static solution check (Dynamic Relaxation)
		DynamicRelaxation::setStaticSolution(particles);

		// Step 13: Advance simulation time
		ModelSetup::setCurrentTime(iTime += dt);
	}

	// Final output
	Output::writeGrid(mesh, Output::CELLS);
	Output::writeResultsSeries();
}