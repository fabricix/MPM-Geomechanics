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

#include <vector>
using std::vector;

#include <iostream>
using std::cout;

SolverExplicitTwoPhaseUSL::SolverExplicitTwoPhaseUSL() : Solver() {}

void SolverExplicitTwoPhaseUSL::Solve()
{

	// check if is two-phase calculations
	if (!ModelSetup::getTwoPhaseActive()) return;

	// simulation variables
	double time = ModelSetup::getTime();
	double dt = ModelSetup::getTimeStep();
	int resultSteps = ModelSetup::getResultSteps();
	double iTime = 0.0;
	int loopCounter = 0;
	ModelSetup::setLoopCounter(loopCounter);

	// write initial particles and grid states
	Output::writeInitialState(bodies, iTime, mesh);

	// solve in time
	while (iTime < time)
	{
		// increment loop counter
		loopCounter = ModelSetup::incrementLoopCounter();

		// advance in time
		ModelSetup::setCurrentTime(iTime += dt);

		// update contribution nodes
		Update::contributionNodes(mesh, particles);

		// nodal mass of solid
		Interpolation::nodalMass(mesh, particles);

		// nodal mass of fluid
		Interpolation::nodalMassFuid(mesh, bodies);

		// impose momentum boundary condition in solid phase
		Update::boundaryConditionsMomentum(mesh);

		// impose momentum boundary condition in fluid phase
		Update::boundaryConditionsMomentumFluid(mesh);
		
		// nodal internal force of mixture
		Interpolation::nodalInternalForce(mesh, particles);

		// nodal internal force of fluid phase
		Interpolation::nodalInternalForceFluid(mesh, bodies);

		// nodal external force of mixture	
		Interpolation::nodalExternalForce(mesh, particles);

		// nodal external force of fluid phase
		Interpolation::nodalExternalForceFluid(mesh, bodies);

		// nodal drag force of fluid
		Interpolation::nodalDragForceFluid(mesh, bodies);

		// nodal total force in mixture
		Update::nodalTotalForce(mesh);

		// impose force boundary conditions
		Update::boundaryConditionsForce(mesh);

		// impose force boundary conditions
		Update::boundaryConditionsForceFluid(mesh);

		// integrate the grid nodal momentum equation in mixture
		Integration::nodalMomentum(mesh, loopCounter == 1 ? dt / 2.0 : dt);

		// update particle velocity of solid phase
		Update::particleVelocity(mesh, particles, loopCounter == 1 ? dt / 2.0 : dt);

		// update particle velocity of fluid phase
		Update::particleVelocityFluid(mesh, particles, loopCounter == 1 ? dt / 2.0 : dt);

		// update particle position of solid phase
		Update::particlePosition(mesh, particles, dt);

		// nodal velocity
		Update::nodalVelocity(mesh);

		// calculate particle strain increment
		Interpolation::particleStrainIncrement(mesh, particles, dt);

		// calculate particle strain increment of fluid phase
		Interpolation::particleStrainIncrementFluid(mesh, bodies, dt);

		// calculate particle vorticity increment	
		Interpolation::particleVorticityIncrement(mesh, particles, dt);

		// calculate particle deformation gradient
		Interpolation::particleDeformationGradient(mesh, particles, dt);

		// update particle density
		Update::particleDensity(particles);

		// update particle porosity
		Update::particlePorosity(particles);

		// update particle pressure
		Update::particlePressure(bodies, dt);

		// update particle stress
		Update::particleStress(particles);

		// write particles and grid in step
		Output::writeResultInStep(resultSteps, particles, iTime);
		Output::writeGridInStep(resultSteps, mesh, iTime);

		// reset all nodal values
		Update::resetNodalValues(mesh);

		// Compute current kinetic energy
		Energy::computeKineticEnergy(particles);

		// verify the static solution requirements
		DynamicRelaxation::setStaticSolution(particles);

		// advance in time
		ModelSetup::setCurrentTime(iTime += dt);
	}

	// write results series
	Output::writeResultsSeries();
}