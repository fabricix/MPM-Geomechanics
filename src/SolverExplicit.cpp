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
	// Initialization simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	bool useMUSL = (ModelSetup::getUpdateStressScheme() == ModelSetup::MUSL);
	bool useSTLContact = ModelSetup::getTerrainContactActive();
	bool contactActive = ModelSetup::getContactActive();
	bool twoPhaseActive = ModelSetup::getTwoPhaseActive();
	int loopCounter = 0;
	ModelSetup::setLoopCounter(loopCounter);

	// Write initial particles and grid states
	Output::writeInitialState(bodies, iTime, mesh, terrainContact);

	// Time integration loop
	while (iTime < time)
	{
		// Increment loop counter
		loopCounter = ModelSetup::incrementLoopCounter();

		// Update contribution nodes
		Update::contributionNodes(mesh, particles);

		// First contact detection
		if (contactActive) { contactManager->contactNodalDetection(mesh, bodies); }

		// Nodal mass and momentum
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			Interpolation::nodalMass(mesh, particles);

			#pragma omp section
			Interpolation::nodalMomentum(mesh, particles);
		}

		if (twoPhaseActive) {
			Interpolation::nodalMassFluid(mesh, bodies);
			Interpolation::nodalMomentumFluid(mesh, bodies);
		}

		// Nodal unit normal for contact
		if (contactActive) { contactManager->nodalUnitNormal(mesh, bodies);	}

		// Update seismic velocity and acceleration from record
		Seismic::updateSeismicVectors(iTime, loopCounter == 1 ? dt / 2.0 : dt);

		// Impose boundary conditions on nodal momentum
		Update::boundaryConditionsMomentum(mesh);

		// Impose momentum boundary condition in fluid phase
		if (twoPhaseActive) { Update::boundaryConditionsMomentumFluid(mesh);
		}

		// Nodal internal forces
		Interpolation::nodalInternalForce(mesh, particles);

		if (twoPhaseActive) { Interpolation::nodalInternalForceFluid(mesh, particles); }

		// Nodal external forces
		Interpolation::nodalExternalForce(mesh, particles);

		if (twoPhaseActive) {
			// nodal external force of fluid phase
			Interpolation::nodalExternalForceFluid(mesh, bodies);

			// nodal drag force of fluid
			Interpolation::nodalDragForceFluid(mesh, bodies);
		}

		// Total force nodal force
		Update::nodalTotalForce(mesh);

		// Impose boundary conditions on total force
		Update::boundaryConditionsForce(mesh);

		if (twoPhaseActive) { Update::boundaryConditionsForceFluid(mesh); }

		// Integrate nodal momentum
		Integration::nodalMomentum(mesh, loopCounter == 1 ? dt / 2.0 : dt);

		// Apply multi field velocity contact correction
		if (contactActive) {
			contactManager->applyVelocityContactCorrection(mesh, bodies, loopCounter == 1 ? dt / 2.0 : dt);
		}

		// Update particle velocity
		Update::particleVelocity(mesh, particles, loopCounter == 1 ? dt / 2.0 : dt);

		if (twoPhaseActive) { Update::particleVelocityFluid(mesh, particles, loopCounter == 1 ? dt / 2.0 : dt);	}

		// Apply contact correction in particle velocity
		if (useSTLContact){

			// Apply seismic velocity to marked nodes
			Seismic::applySeismicVelocityMarkedSTLNodes(mesh);

			// Apply velocity contact correction
			terrainContact->apply(mesh, particles, dt);
		}

		// Update particle position
		Update::particlePosition(mesh, particles, dt);

		// Project particle position if any penetrations
		if (useSTLContact) terrainContact->projectParticles(mesh, particles);

		// (MUSL) Momentum recalculation
		if (useMUSL)
		{
			// Reset nodal momentum
			Update::resetNodalMomentum(mesh);
			if (twoPhaseActive) { Update::resetNodalMomentumFluid(mesh); }

			// update contact momentum
			if (contactActive) { contactManager->nodalMomentumContactUpdate(mesh, dt); }

			// Recalculate nodal momentum with nodal velocities
			Interpolation::nodalMomentum(mesh, particles);
			if(twoPhaseActive){Interpolation::nodalMomentumFluid(mesh, bodies);}

			// Reapply BCs on nodal momentum
			Update::boundaryConditionsMomentum(mesh);
			
			if (twoPhaseActive) {
				// Impose force boundary conditions
				Update::boundaryConditionsMomentumFluid(mesh);
			}
		}

		// Compute nodal velocity
		Update::nodalVelocity(mesh);

		// Update strain increments
		Interpolation::particleStrainIncrement(mesh, particles, dt);

		if (twoPhaseActive) { Interpolation::particleStrainIncrementFluid(mesh, bodies, dt); }

		// Update vorticity increments
		Interpolation::particleVorticityIncrement(mesh, particles, dt);

		// Update density
		Update::particleDensity(particles);

		if (twoPhaseActive) {
			// calculate particle deformation gradient
			Interpolation::particleDeformationGradient(mesh, particles, dt);

			// update particle porosity
			Update::particlePorosity(particles);

			// update particle pressure
			Update::particlePressure(bodies, dt);
		}

		// Update stress
		Update::particleStress(particles);

		// Reset nodal values
		Update::resetNodalValues(mesh);

		// Compute current kinetic energy
		Energy::computeKineticEnergy(particles);

		// Static solution using Dynamic Relaxation
		DynamicRelaxation::setStaticSolution(particles);

		// Advance simulation time
		ModelSetup::setCurrentTime(iTime += dt);

		// Rrite results in step
		Output::writeResultInStep(resultSteps, particles, iTime);
		Output::writeGridInStep(resultSteps, mesh, iTime);
		Output::writeSTLContactInStep(resultSteps, terrainContact, iTime);
	}

	// Final output
	Output::writeGrid(mesh, Output::CELLS);
	Output::writeResultsSeries();
}