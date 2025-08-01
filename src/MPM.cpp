// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "MPM.h"
#include "Warning.h"
#include "Solver/SolverExplicitUSL.h"
#include "Warning.h"
#include "Materials/Elastic.h"
#include "Body/BodyCuboid.h"
#include "Shape/ShapeGimp.h"
#include "Shape/ShapeLinear.h"
#include "Loads.h"
#include "TerrainContact.h"
#include "HydroMechanicalCoupling.h"
#include "Seismic.h"

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
	solver->registerParticles(&particles);
	solver->registerTerrainContact(terrainContact);
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

	// compute the nodal volumes
	mesh.computeNodeVolumes();

	// configure the mesh boundary conditions
	mesh.setBoundaryRestrictions(Input::getMeshBoundaryConditions());

	if(ModelSetup::getSeismicAnalysisActive()){
		// set the boundary conditions for seismic analysis
		mesh.setBoundaryRestrictionsSeismic();
	}
	if (ModelSetup::getTwoPhaseActive()){
		// configure the mesh boundary conditions of fluid
		mesh.setBoundaryRestrictionsFluid(Input::getMeshBoundaryConditionsFluid());		
	}
}

void MPM::setupTerrainContact()
{
	// verity if terrain contact active
	bool terrainContactActive = Input::getTerrainContactActive();
	ModelSetup::setTerrainContactActive(terrainContactActive);
	if (!terrainContactActive) { return;}

	// configure STL mesh for terrain contact
	string stlMeshFile = Input::getSTLMeshFile();
	
	if (stlMeshFile!=""){
	
		// create mesh pointer
		STLReader* stlMesh = new STLReader;
		// read STL mesh file
		bool success = stlMesh->read(stlMeshFile);
		// calculate normals
		if (success) stlMesh->recalculateNormals();
		else {
			Warning::printMessage("Error reading STL mesh file: "+stlMeshFile);
			throw (0);
		}
		// write STL mesh file
		if (Input::getWriteSTLMeshFile()){
			stlMesh->writeSTL(stlMeshFile+"_with_normals.stl");
		}
		// filtrate outside triangles
		stlMesh->removeTrianglesOutsideLimits(mesh.getMinLimits(), mesh.getMaxLimits());

		// set stl mesh in terrain contact
		terrainContact = new TerrainContact(stlMesh,Input::getFrictionCoefficient());

		// configure the threshold for contact detection
		terrainContact->setDistanceThreshold(Input::getDistanceThreshold());
		
		// compute distance level set function
		terrainContact->computeDistanceLevelSetFunction(&mesh);

		// mark seismic nodes for STL seismic loading
		if (ModelSetup::getSeismicAnalysisActive() && terrainContact != nullptr)
		{
	    	double epsilon = 0.25 * mesh.getCellDimension().mean();
    		
			// mark seismic nodes based on distance from level set
			// this will mark nodes that are close to the terrain contact surface
			// and will be used to apply seismic loading in the seismic analysis
			Seismic::markSeismicNodes(epsilon, &mesh);

			// disable Zo earthquake treatment for seismic nodes
			mesh.setRestriction(Boundary::BoundaryPlane::Z0, Boundary::BoundaryType::SLIDING);
		}

		// configure penalty contact method
		if (Input::getPenaltyContactActive()) {
			terrainContact->enablePenaltyContact(true);
			terrainContact->setPenaltyStiffness(Input::getPenaltyStiffness());
		}
		else {
			terrainContact->enablePenaltyContact(false);
		}	

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

	// clear particle list
	particles.clear();

	// set body id and shape function
	for (size_t i = 0; i < bodies.size(); ++i){

		vector<Particle*>* particles = bodies.at(i)->getParticles();

		for (size_t j = 0; j < particles->size(); ++j){

			// set body id
			particles->at(j)->setBodyId(bodies.at(i)->getId());

			// set shape function
			switch(ModelSetup::getInterpolationFunction())
			{
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

			// register particle is material point list
			this->particles.push_back(particles->at(j));
		}
	}
}

void MPM::setupLoads() {

	// gravity
	ModelSetup::setGravity(Input::getGravity());
	Loads::setGravity(bodies);

	// set external forces in particles
	Loads::setLoadDistributedBox(bodies, Input::getLoadDistributedBox());

	// set nodal loads
	Loads::configureNodalPointLoads(&mesh);

	// set initial pore pressure
	Loads::setInitialPorePressureBox(bodies, Input::getInitialPressureBox());
	Loads::setInitialPorePressureMaterial(bodies, Input::getInitialPressureMaterial());
	
	// set prescribed pore pressures
	Loads::setPrescribedPorePressureBox(bodies, Input::getPrescribedPressureBox());
	
	// set traction pore pressure forces
	Loads::setPrescribedPorePressureBoundaryForceBox(bodies, Input::getPressureBoundaryForceBox());

	// set initial velocity
	Loads::setInitialVelocity(bodies);
}

void MPM::setupSeismicAnalysis() {

	// set seismic analysis info
	Seismic::setSeismicAnalysis(Input::getSeismicAnalysisInfo());

	// configure seismic analysis in mpm model
	if(!Seismic::getSeismicAnalysis().isActive) return;
	
	ModelSetup::setSeismicAnalysisActive(true);

	// setup seismic data
	Seismic::setSeismicData();
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

void MPM::loadState()
{
	ModelSetup::setLoadState(Input::getLoadState());

	if ( ModelSetup::getLoadState() ) 
	{
		States::loadParticleStress("particle_stress_data.json", particles);
	}
}

void MPM::saveState()
{
	ModelSetup::setSaveState(Input::getSaveState());

	if ( ModelSetup::getSaveState() )
	{
		States::saveParticleStress("particle_stress_data.json", particles);
	}
}

void MPM::setOneDirectionHydromechanicalCoupling()
{
	HydroMechanicalCoupling::configureOneDirectionCoupling(particles);
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

		// configures the seismic analysis
		setupSeismicAnalysis();

		// setup the background mesh
		setupMesh();

		// setup terrain contact
		setupTerrainContact();

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

		// load previous state
		loadState();

		// configures the loads
		setupLoads();

		// configures the hydro-mechanical coupling type
		setOneDirectionHydromechanicalCoupling();

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
		ModelSetup::setInitialSimulationTime(std::chrono::system_clock::now());
		solver->Solve();
	}
}

void MPM::end() { Output::printElapsedTime(); }