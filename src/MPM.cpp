/*
 * MPM.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "MPM.h"
#include "Warning.h"
#include "SolverUSL.h"
#include "Warning.h"
#include "Elastic.h"
#include "BodyCuboid.h"
#include "ShapeGimp.h"
#include "ShapeLinear.h"
#include "Loads.h"

#include "Json/json.hpp"
using json = nlohmann::json;

#include <chrono>

#include <iostream>
using std::cout;

#include<string>
using std::to_string;

MPM::MPM() {
	
	solver=0;
}

MPM::~MPM() {

}

bool MPM::readInputFile(int argc, char **argv){
	
	if (argc!=2){

		Warning::printMessage("Bad argument list");
		return false;
	}
	else{

		Input::readInputFile(string(argv[1]));
		return true;
	}
	return false;
}

void MPM::setSimulationTime(){
	
	ModelSetup::setTime(Input::getSimulationTime());
}

void MPM::setSolver() {
	
	solver = Input::getSolver();
}

void MPM::setInterpolationFunctions() {

	ModelSetup::setInterpolationFunction(Input::getInterpolationFunction());
}

void MPM::setTimeStep(){

	// input time step
	ModelSetup::setTimeStep(Input::getTimeStep());

	// verify the Courant-Friedrichs-Lewy condition
	
	// maximum sound speed
	double maxSoundSpeed = 0.0;
	for (size_t i = 0; i < materials.size(); ++i){

		double currentSoundSpeed = materials.at(i)->getSoundSpeed();
		
		if (currentSoundSpeed>maxSoundSpeed) {
		
			maxSoundSpeed=currentSoundSpeed;
		}
	}

	// mean cells dimensions
	double cellsDimention = mesh.getCellDimension().mean();

	// critical time step
	if (maxSoundSpeed!=0.0)
	{
		double criticalTimeStep = cellsDimention/maxSoundSpeed;

		if (ModelSetup::getTimeStep()>criticalTimeStep){

			Warning::printMessage("time step is greater than critical time step: "+to_string(criticalTimeStep));
		}
	}
	else
	{
		Warning::printMessage("verify the material sound speed: "+to_string(maxSoundSpeed));
	}
}

void MPM::setupMesh(){
	
	// number of cells
	mesh.setNumCells(Input::getCellsNum());
	
	// set cell dimension
	mesh.setCellDimension(Input::getCellDimension());
	
	// set origin
	mesh.setOrigin(Input::getOrigin());

	// create the mesh
	mesh.createGrid();
}

void MPM::setupMaterialList(){

	materials=Input::getMaterialList();
}

void MPM::setupBodyList(){

	bodies=Input::getBodyList();
}

void MPM::createBodies(){

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

void MPM::setupParticles(){

	for (size_t i = 0; i < bodies.size(); ++i){

		vector<Particle*>* particles = bodies.at(i)->getParticles();

		for (size_t j = 0; j < particles->size(); ++j){

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
			}
		}
	}
}

void MPM::setupLoads(){

	// gravity
	ModelSetup::setGravity(Input::getGravity());
	Loads::setGravity(bodies);
}

void MPM::setupResults(){

	// gravity
	ModelSetup::setResultNum(Input::getResultNum());
}

void MPM::createModel(){

	// set the simulation time 
	setSimulationTime();
	
	// set the update stress scheme
	setSolver();

	// set the interpolation functions
	setInterpolationFunctions();

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

	// configures the particles in the model
	setupParticles();

	// configures the loads
	setupLoads();

	// configures the results
	setupResults();
}

void MPM::solve(){
	
	if (solver==0){

		Warning::printMessage("The solver was not correctly defined.");
	}
	else{

		auto start = std::chrono::system_clock::now();

		solver->Solve(bodies, mesh);

		auto end = std::chrono::system_clock::now();
		
		std::chrono::duration<double> elapsed_seconds = end-start;
	
		cout<<"Elapsed time: "<<elapsed_seconds.count()<<" seconds\n";
	}
}

void MPM::end(){

	cout << "The program finished."<<"\n";
}
