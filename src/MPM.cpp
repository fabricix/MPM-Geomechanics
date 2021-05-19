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

#include "Json/json.hpp"
using json = nlohmann::json;

#include <chrono>

#include <iostream>
using std::cout;


MPM::MPM() {
	solver=0;
}

MPM::~MPM() {
	// TODO Auto-generated destructor stub
}

bool MPM::readInputFile(int argc, char **argv){

	if (argc!=2){

		Warning::printMessage("Bad argument list");
		return false;
	}
	else{

		input.readInputFile(string(argv[1]));
		return true;
	}
	return false;
}

void MPM::setSimulationTime(){
	
	ModelSetup::setTime(input.getSimulationTime());
}

void MPM::setSolver() {
	
	solver = input.getSolver();
}

void MPM::setInterpolationFunctions() {

	ModelSetup::setInterpolationFunction(input.getInterpolationFunction());
}

void MPM::setTimeStep(){

	ModelSetup::setTimeStep(input.getTimeStep());
}

void MPM::setUpMesh(){
	
	// number of cells
	mesh.setNumCells(input.getCellsNum());
	
	// set cell dimension
	mesh.setCellDimension(input.getCellDimension());
	
	// set origin
	mesh.setOrigin(input.getOrigin());

	// create the mesh
	mesh.createGrid();
}

void MPM::setUpMaterialList(){

	materials=input.getMaterialList();
}

void MPM::setUpBodyList(){

	bodies=input.getBodyList();
}

void MPM::createBodies(){

	for (size_t i = 0; i < bodies.size(); ++i)
	{	
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

void MPM::setUpParticles(){

	for (size_t i = 0; i < bodies.size(); ++i)
	{	
		vector<Particle*>& particles = bodies.at(i)->getParticles();

		for (size_t j = 0; j < particles.size(); ++j)
		{
			// set shape function
			switch(ModelSetup::getInterpolationFunction())
			{
				case ModelSetup::LINEAR:
					particles.at(j)->setShape(new ShapeLinear);
					break;

				case ModelSetup::GIMP:
					particles.at(j)->setShape(new ShapeGimp);
					break;
				default:
				Warning::printMessage("Bad definition of shape function in particle");
			}
		}
	}
}

void MPM::createModel(){

	// set the simulation time 
	setSimulationTime();

	// set time step
	setTimeStep();
	
	// set the update stress scheme
	setSolver();

	// set the interpolation functions
	setInterpolationFunctions();

	// setup the mesh
	setUpMesh();

	// setup the material list
	setUpMaterialList();

	// setup the body list
	setUpBodyList();

	// create the bodies
	createBodies();

	// configures the particles in the model
	setUpParticles();
}

void MPM::solve(){
	
	if (solver==0)
	{
		Warning::printMessage("The solver was not correctly defined.");
	}
	else{

		auto start = std::chrono::system_clock::now();

		solver->Solve(bodies, mesh);

		auto end = std::chrono::system_clock::now();
		
		std::chrono::duration<double> elapsed_seconds = end-start;
	
		cout << "Elapsed time: " << elapsed_seconds.count()<<  "seconds";
	}
	
}

void MPM::end(){

	cout << "The program finished."<<"\n";
}
