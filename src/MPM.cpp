/*
 * MPM.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "MPM.h"
#include "Warning.h"
#include "Solver/SolverExplicitUSL.h"
#include "Warning.h"
#include "Materials/Elastic.h"
#include "Body/BodyCuboid.h"
#include "Shape/ShapeGimp.h"
#include "Shape/ShapeLinear.h"
#include "Loads.h"

#include "Json/json.hpp"
using json = nlohmann::json;

#include <chrono>

#include <iostream>
using std::cout;

#include<string>
using std::to_string;

#include <cstdlib>
using std::exit;

MPM::MPM() {
	
	solver=0;
}

MPM::~MPM() { }

void MPM::readInputFile(int argc, char **argv) {
	
	try{
		if (argc!=2){

			Warning::printMessage("Bad argument list");
		}
		else{

			Input::readInputFile(string(argv[1]));
		}
	}
	catch(...)
	{	
		Warning::printMessage("Error during reading the input file");
		Warning::printMessage("The program finished");
		Output::farewellScreen();
		exit(EXIT_FAILURE);
	}
}

void MPM::setSimulationTime() {
	
	ModelSetup::setTime(Input::getSimulationTime());
	
	if(ModelSetup::getTime()<=0.0){
		
		Warning::printMessage("Simulation time must be greater that zero");
		throw (0);
	}
}

void MPM::setSolver() {
	
	solver = Input::getSolver();
	solver->registerMesh(&mesh);
	solver->registerBodies(&bodies);
}

void MPM::setInterpolationFunctions() {

	ModelSetup::setInterpolationFunction(Input::getInterpolationFunction());
}

void MPM::setTimeStep() {

	// time step
	ModelSetup::setTimeStep(Input::getTimeStep());

	// critical time step multiplier
	ModelSetup::setCriticalTimeStepMultiplier(Input::getCriticalTimeStepMultiplier()>0 ? Input::getCriticalTimeStepMultiplier(): ModelSetup::getCriticalTimeStepMultiplier());

	// verify the Courant-Friedrichs-Lewy condition
	
	// maximum sound speed of materials
	double maxSoundSpeed = 0.0;
	for (size_t i = 0; i < materials.size(); ++i){

		double currentSoundSpeed = materials.at(i)->getSoundSpeed();
		
		if (currentSoundSpeed>maxSoundSpeed) { maxSoundSpeed=currentSoundSpeed;	}
	}

	// mean cells dimensions
	double cellsDimention = mesh.getCellDimension().mean();

	// critical time step
	if (maxSoundSpeed!=0.0)
	{
		double criticalTimeStep = cellsDimention/maxSoundSpeed;

		if (ModelSetup::getTimeStep()!=0 && ModelSetup::getTimeStep()>criticalTimeStep){

			Warning::printMessage("time step is greater than critical time step: "+to_string(criticalTimeStep));
		}

		else if (ModelSetup::getTimeStep()==0)
		{
			// set the default time step using critical time step multiplier
			ModelSetup::setTimeStep(ModelSetup::getCriticalTimeStepMultiplier()*criticalTimeStep);
		}
	}
	else
	{
		Warning::printMessage("verify the material sound speed: "+to_string(maxSoundSpeed));
		throw (0);
	}
}

void MPM::setupMesh() {
	
	// number of cells
	mesh.setNumCells(Input::getCellsNum());
	
	// set cell dimension
	mesh.setCellDimension(Input::getCellDimension());
	
	// set origin
	mesh.setOrigin(Input::getOrigin());

	// create the mesh
	mesh.createGrid(ModelSetup::getTwoPhaseActive());

	// configure the mesh boundary conditions
	mesh.setBoundaryRestrictions(Input::getMeshBoundaryConditions());

	if (ModelSetup::getTwoPhaseActive()){

		// configure the mesh boundary conditions of fluid
		mesh.setBoundaryRestrictionsFluid(Input::getMeshBoundaryConditionsFluid());		
	}
}

void MPM::setupMaterialList() {

	materials=Input::getMaterialList();
}

void MPM::setupBodyList() {

	bodies=Input::getBodyList();
}

void MPM::createBodies() {

	for (size_t i = 0; i < bodies.size(); ++i){

		// get material from list
		Material* iMaterial=0;
		for (size_t j = 0; j < materials.size(); ++j) {

			if (bodies.at(i)->getMaterialId()==materials.at(j)->getId()){

				iMaterial=materials.at(j);
				break;
			}
		}
		bodies.at(i)->create(mesh,iMaterial);
	}
}

void MPM::setupParticles() {

	for (size_t i = 0; i < bodies.size(); ++i){

		vector<Particle*>* particles = bodies.at(i)->getParticles();

		for (size_t j = 0; j < particles->size(); ++j){

			// set body id
			particles->at(j)->setBodyId(bodies.at(i)->getId());

			// set shape function
			switch(ModelSetup::getInterpolationFunction()){

				case ModelSetup::LINEAR:
					particles->at(j)->setShape(new ShapeLinear);
					break;

				case ModelSetup::GIMP:
					particles->at(j)->setShape(new ShapeGimp);
					break;
				default:
				Warning::printMessage("Bad definition of shape function in particle");
				throw (0);
			}
		}
	}
}

void MPM::setupLoads() {

	// gravity
	ModelSetup::setGravity(Input::getGravity());
	Loads::setGravity(bodies);

	// set external forces
	Loads::setLoadDistributedBox(bodies, Input::getLoadDistributedBox());

	// set initial pore pressure
	Loads::setInitialPorePressureBox(bodies, Input::getInitialPressureBox());
	Loads::setInitialPorePressureMaterial(bodies, Input::getInitialPressureMaterial());
	
	// set prescribed pore pressures
	Loads::setPrescribedPorePressureBox(bodies, Input::getPrescribedPressureBox());
	
	// set traction pore pressure forces
	Loads::setPrescribedPorePressureBoundaryForceBox(bodies, Input::getPressureBoundaryForceBox());
}

void MPM::setupDamping() {

	// set damping type
	ModelSetup::setDampingType(Input::getDampingType());

	// set local damping value
	if (ModelSetup::getDampingType()==ModelSetup::LOCAL){

		ModelSetup::setDampingLocalValue(Input::getDampingValue());
	}
}

void MPM::setupResults() {

	// number of results
	ModelSetup::setResultNum(Input::getResultNum());
	
	// configures the fields
	Output::configureResultFiels(Input::getResultFields());
}

void MPM::setThreads() {

	// configures the number of threads
	ModelSetup::setNumThreads(Input::getNumThreads());
}

void MPM::setNumberPhasesInSimulation() {

	// configures the number of phases in the simulation
	ModelSetup::setTwoPhaseActive(Input::getNumberPhases()>1?true:false);
}

void MPM::createModel() {

	try{
		// set number of threads
		setThreads();

		// set the simulation time 
		setSimulationTime();
		
		// set the interpolation functions
		setInterpolationFunctions();

		// set number of phases in the simulations
		setNumberPhasesInSimulation();

		// setup the mesh
		setupMesh();

		// setup the material list
		setupMaterialList();

		// set time step
		setTimeStep();
		
		// setup the body list
		setupBodyList();

		// create the bodies
		createBodies();

		// setup the solver
		setSolver();

		// configures the particles in the model
		setupParticles();

		// configures the loads
		setupLoads();

		// configures the damping
		setupDamping();

		// configures the results
		setupResults();

	}
	catch(...)
	{
		Warning::printMessage("Error during model creation");
		Warning::printMessage("Verify the input file");
		Warning::printMessage("The program finished");
		Output::farewellScreen();
		exit(EXIT_FAILURE);
	}
}

void MPM::solve(){
	
	if (solver==0){

		Warning::printMessage("The solver was not correctly defined");
		Warning::printMessage("The program finished");
		Output::farewellScreen();
		exit(EXIT_FAILURE);
	}
	else{

		auto start = std::chrono::system_clock::now();

		solver->Solve();

		auto end = std::chrono::system_clock::now();
		
		std::chrono::duration<double> elapsed_seconds = end-start;
	
		cout<<"\nElapsed time: "<<elapsed_seconds.count()<<" seconds\n";
	}
}

void MPM::end(){

}
