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
#include "Parallelization.h"

#include "Json/json.hpp"
using json = nlohmann::json;

#include <chrono>

#include <iostream>
using std::cout;

#include<string>
using std::to_string;

#include <cstdlib>
using std::exit;

#include <cmath>

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

void MPM::setupParticlesPerThread()
{
	// Initialize the particles per thread vector
	solver->particlesPerThread = new vector<vector<Particle*>*>();

	// Calculate the number of particles per thread
	Parallelization::calculateParticlesPerThread(solver->getMesh(), *solver->particlesPerThread, particles, ModelSetup::getPartitionFactor());
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

	if(ModelSetup::getSeismicAnalysis()){
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

	Vector3d minPoint;	
	Vector3d maxPoint;

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

			minPoint = minPoint.cwiseMin(particles->at(j)->getPosition());
			maxPoint = maxPoint.cwiseMax(particles->at(j)->getPosition());
		}
	}
	//cout << "punto minimo: " << minPoint << "punto maximo: " << maxPoint;
	BoundingBox box = BoundingBox(minPoint,maxPoint,Input::getNumThreads());
	this->setBoundingBox(box);
}

void MPM::setupSubdomains(){
	BoundingBox box = this->boundingBox;
    float width = box.getWidth();
	double frontier = width/2;

	float particleMeanPerDomain = particles.size()/boundingBox.getSubdomainsNumber();
    //float frontier = width/this->boundingBox.getSubdomainsNumber();
	//Tolerancia de cuantas particulas pueden sobrepasar 0 a 1 (%)
	float tolerance = 0.1; //10%

	vector<Particle*> subdomain1 = std::vector<Particle*>(particles.size());
	vector<Particle*> subdomain2 = std::vector<Particle*>(particles.size());

    //recorremos las particulas
	//for (size_t i = 0; i < this->particles.size(); ++i){
	for (size_t i = 0; i < particles.size(); ++i){
		Vector3d position = particles.at(i)->getPosition();
		if(position.x() < frontier){
			//almacenamos las particulas en el subdominio izquierdo
			//box.setSubdomainParticle(0,particles.at(i));
			subdomain1.push_back(particles.at(i));
			continue;
		}

		//box.setSubdomainParticle(1,particles.at(i));
		subdomain2.push_back(particles.at(i));
	}

    // Llamamos a la función recursiva para rebalancear los subdominios
    //rebalanceSubdomains(frontier, particleMeanPerDomain, tolerance, box, subdomain1, subdomain2);
}

void MPM::rebalanceSubdomains(float frontier, float particleMeanPerDomain, float tolerance, BoundingBox& box, vector<Particle*> subdomain1,vector<Particle*> subdomain2) {
    // Verificamos el número de partículas en cada subdominio
    int subdomain0Size = box.getSubdomainSize(0);
    int subdomain1Size = box.getSubdomainSize(1);

    //cout << "Tamaño subdominio 1: " << subdomain0Size << " | Tamaño subdominio 2: " << subdomain1Size;

    // Si los subdominios están equilibrados, terminamos
    if (fabs(subdomain1.size() - particleMeanPerDomain) <= tolerance * particleMeanPerDomain &&
        fabs(subdomain2.size() - particleMeanPerDomain) <= tolerance * particleMeanPerDomain) {
        return;  // Base case: el rebalanceo ya está hecho
    }

	//Limpiamos subdominios
	//subdomain1 = std::vector<Particle*>(particles.size());
	//subdomain2 = std::vector<Particle*>(particles.size());

    // Si el subdominio 0 tiene más partículas, movemos la frontera hacia la derecha
    if (subdomain1.size() > particleMeanPerDomain * (1 + tolerance)) {
        frontier += 0.1 * box.getWidth();  // Mover la frontera hacia la derecha

        // Reasignamos las partículas basándonos en la nueva frontera
        for (size_t i = 0; i < particles.size(); ++i) {
            Vector3d position = particles.at(i)->getPosition();
            if (position.x() < frontier) {
                subdomain1.push_back(particles.at(i));  // Subdominio izquierdo
            } else {
                subdomain2.push_back(particles.at(i));  // Subdominio derecho
            }
        }
    }

    // Si el subdominio 1 tiene más partículas, movemos la frontera hacia la izquierda
    if (subdomain2.size() > particleMeanPerDomain * (1 + tolerance)) {
        frontier -= 0.1 * box.getWidth();  // Mover la frontera hacia la izquierda

        // Reasignamos las partículas basándonos en la nueva frontera
        for (size_t i = 0; i < particles.size(); ++i) {
            Vector3d position = particles.at(i)->getPosition();
            if (position.x() < frontier) {
                subdomain1.push_back(particles.at(i));  // Subdominio izquierdo
            } else {
                subdomain2.push_back(particles.at(i));  // Subdominio derecho
            }
        }
    }
	//cout << "Tamaño subdominio 1: " << subdomain1.size() << " | Tamaño subdominio 2: " << subdomain2.size();
    // Llamamos recursivamente para intentar equilibrar nuevamente
    rebalanceSubdomains(frontier, particleMeanPerDomain, tolerance, box, subdomain1, subdomain2);
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
	
	ModelSetup::setSeismicAnalysis(true);

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
	Output::configureGridResultFiels(Input::getGridResultFields());
}

void MPM::setThreads() {

	// configures the number of threads
	ModelSetup::setNumThreads(Input::getNumThreads());
}

void MPM::setPartitionFactor() {

	// configures the number of threads
	ModelSetup::setPartitionFactor(Input::getPartitionFactor());
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

		//configure subdomains
		setupSubdomains();

		// load previous state
		loadState();

		// configures the loads
		setupLoads();

		// configures the damping
		setupDamping();

		// configures the results
		setupResults();

		// set partition factor
		setPartitionFactor();

		// load particles per thread in the solver
		setupParticlesPerThread();

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