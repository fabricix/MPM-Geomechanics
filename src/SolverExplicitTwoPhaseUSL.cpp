// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Solver/SolverExplicitTwoPhaseUSL.h"
#include "Model.h"
#include "Integration.h"
#include "Interpolation.h"
#include "Update.h"
#include "Output.h"
#include "DynamicRelaxation.h"
#include "Energy.h"

SolverExplicitTwoPhaseUSL::SolverExplicitTwoPhaseUSL() : Solver() {}

// TODO: Merge this function with SolverExplicit::Solve() and use the same function for both solvers. The differences between the two solvers can be handled by if conditions inside the function.
void SolverExplicitTwoPhaseUSL::Solve()
{
	// Check for two-phase calculations
	if (!ModelSetup::getTwoPhaseActive()) return;

	// Initialization of simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	int loopCounter = 0;
	Vector3d gravity = ModelSetup::getGravity();
	ModelSetup::setLoopCounter(loopCounter);

	// Write initial particles and grid state
	Output::writeInitialState(bodies, iTime, mesh, nullptr);

	// Time integration loop
	while (iTime < time)
	{
		// increment loop counter
		loopCounter = ModelSetup::incrementLoopCounter();

		// Step 0: Update contribution nodes
		Update::contributionNodes(mesh, particles);

		// Step 1.a: Interpolate nodal mass of solid and fluid phases
		Interpolation::nodalMass(mesh, particles);
		Interpolation::nodalMassFluid(mesh, particles);
	
		// Step 1.b: Interpolate nodal momentum
		Interpolation::nodalMomentum(mesh, particles);
		Interpolation::nodalMomentumFluid(mesh, particles);

		// Step 2: Impose momentum boundary conditions
		Update::boundaryConditionsMomentum(mesh);
		Update::boundaryConditionsMomentumFluid(mesh);
		
		// Step 3.1: Internal nodal force
		Interpolation::nodalInternalForce(mesh, particles);
		Interpolation::nodalInternalForceFluid(mesh, particles);

		// Step 3.2: External nodal force
		Interpolation::nodalExternalForce(mesh, particles);
		Interpolation::nodalExternalForceFluid(mesh, particles);

		// Step 3.3: nodal drag force
		Interpolation::nodalDragForceFluid(mesh, particles, gravity);

		// Step 3.4: Total nodal force
		Update::nodalTotalForce(mesh);

		// Step 3.5: Impose boundary conditions on forces
		Update::boundaryConditionsForce(mesh);
		Update::boundaryConditionsForceFluid(mesh);

		// Step 4: Integrate nodal momentum
		Integration::nodalMomentum(mesh, loopCounter == 1 ? dt / 2.0 : dt);

		// Step 5.1: Update velocity
		Update::particleVelocity(mesh, particles, loopCounter == 1 ? dt / 2.0 : dt);
		Update::particleVelocityFluid(mesh, particles, loopCounter == 1 ? dt / 2.0 : dt);

		// Step 5.3: Update particle position
		Update::particlePosition(mesh, particles, dt);

		// Step 6: Nodal velocity
		Update::nodalVelocity(mesh);

		// Step 7.1: Update strain increments
		Interpolation::particleStrainIncrement(mesh, particles, dt);
		Interpolation::particleStrainIncrementFluid(mesh, particles, dt);

		// Step 7.2: Update vorticity increments
		Interpolation::particleVorticityIncrement(mesh, particles, dt);

		// Step 7.3: Update deformation gradient
		Interpolation::particleDeformationGradient(mesh, particles, dt);

		// Step 7.4 Update porosity
		Update::particlePorosity(particles);

		// Step 8.1: Update pressure
		Update::particlePressure(particles, dt);

		// Step 8.2: Update stress
		Update::particleStress(particles);

		// Step 9.0: Write results
		Output::writeResultInStep(resultSteps, particles, iTime);
		Output::writeGridInStep(resultSteps, mesh, iTime);

		// Step 10: Reset nodal values
		Update::resetNodalValues(mesh);

		// Step 11: Compute current kinetic energy
		Energy::computeKineticEnergy(particles);

		// Step 12: Verify the static solution requirements
		DynamicRelaxation::setStaticSolution(particles);

		// Step 13: Advance simulation time
		ModelSetup::setCurrentTime(iTime += dt);
	}

	// write results series
	Output::writeGrid(mesh, Output::CELLS);
	Output::writeResultsSeries();
}