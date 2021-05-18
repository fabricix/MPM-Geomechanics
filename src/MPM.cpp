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

bool MPM::setSimulationTime(){
	
	json inputFile=input.getJson();
	map<Input::KeyWords,string> keywords = input.getKeyWords();

	bool flag = false;

	if(inputFile.contains(keywords[Input::time])&&inputFile[keywords[Input::time]].is_number())
	{
		ModelSetup::setTime(inputFile[keywords[Input::time]]);
		return !flag;
	}
	else
	{
		Warning::printMessage("Verify "+keywords[Input::time]+" definition in the input file");
		return flag;
	}
	return flag;
}

bool MPM::setSolver() {
	
	json inputFile=input.getJson();
	map<Input::KeyWords,string> keywords = input.getKeyWords();
	
	bool flag = false;

	if(inputFile.contains(keywords[Input::stressSchemeUpdate]))
	{
		if(inputFile[keywords[Input::stressSchemeUpdate]]==keywords[Input::USL]) {
			solver = new SolverUSL();
			return !flag;
		}
	}
	
	Warning::printMessage("Verify "+keywords[Input::stressSchemeUpdate]+" definition in the input file");
	return flag;
}

bool MPM::setInterpolationFunctions() {
	
	json inputFile=input.getJson();
	map<Input::KeyWords,string> keywords = input.getKeyWords();
	
	bool flag = false;

	if(inputFile.contains(keywords[Input::shapeFunction]))
	{
		if(inputFile[keywords[Input::shapeFunction]]==keywords[Input::GIMP]) {
			ModelSetup::setInterpolationFunction(ModelSetup::GIMP);
			return !flag;
		}

		if(inputFile[keywords[Input::shapeFunction]]==keywords[Input::linear]) {
			ModelSetup::setInterpolationFunction(ModelSetup::LINEAR);
			return !flag;
		}
	}
	
	Warning::printMessage("Verify "+keywords[Input::shapeFunction]+" definition in the input file");
	return flag;
}

bool MPM::setTimeStep(){

	json inputFile=input.getJson();
	map<Input::KeyWords,string> keywords = input.getKeyWords();

	bool flag = false;

	if(inputFile.contains(keywords[Input::timeStep])&&inputFile[keywords[Input::timeStep]].is_number()){

		ModelSetup::setTimeStep(inputFile[keywords[Input::timeStep]]);
		return !flag;
	}

	Warning::printMessage("Verify "+keywords[Input::timeStep]+" definition in the input file");
	return flag;
}

bool MPM::setUpMesh(){
	
	json inputFile=input.getJson();
	map<Input::KeyWords,string> keywords = input.getKeyWords();

	bool nCellsFlag=false;

	int nCells[3]; 

	if(inputFile.contains(keywords[Input::mesh])&&inputFile[keywords[Input::mesh]].contains(keywords[Input::nCells])){	
		
		if(inputFile[keywords[Input::mesh]][keywords[Input::nCells]].is_array()) {

			nCells[0]=inputFile[keywords[Input::mesh]][keywords[Input::nCells]][0];
			nCells[1]=inputFile[keywords[Input::mesh]][keywords[Input::nCells]][1];
			nCells[2]=inputFile[keywords[Input::mesh]][keywords[Input::nCells]][2];
			mesh.setNumCells(nCells[0],nCells[1],nCells[2]);
			nCellsFlag=!nCellsFlag;
		}
	}
	
	// set cell dimension
	bool cellDimensionFlag=false;

	double cellDimension[3]; 

	if(inputFile.contains(keywords[Input::mesh])&&inputFile[keywords[Input::mesh]].contains(keywords[Input::cellDimension])){

		if(inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]].is_array()) {
		
				cellDimension[0]=inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]][0];
				cellDimension[1]=inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]][1];
				cellDimension[2]=inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]][2];
				mesh.setCellDimension(cellDimension[0],cellDimension[1],cellDimension[2]);
				cellDimensionFlag=!cellDimensionFlag;
		}
	}
	
	// set origin
	bool originFlag = false;

	double origin[3];

	if(inputFile.contains(keywords[Input::mesh])&&inputFile[keywords[Input::mesh]].contains(keywords[Input::origin])){

		if(inputFile[keywords[Input::mesh]][keywords[Input::origin]].is_array()) {
			
				origin[0]=inputFile[keywords[Input::mesh]][keywords[Input::origin]][0];
				origin[1]=inputFile[keywords[Input::mesh]][keywords[Input::origin]][1];
				origin[2]=inputFile[keywords[Input::mesh]][keywords[Input::origin]][2];
				mesh.setOrigin(origin[0],origin[1],origin[2]);
				originFlag=!originFlag;
		}
	}

	if (nCellsFlag && cellDimensionFlag && originFlag){

		mesh.createGrid();
	}
	else{

		Warning::printMessage("The mesh can not be created. Verify the input file:");
		
		if (!nCellsFlag){
			Warning::printMessage("Error in keyword "+keywords[Input::nCells]);
		}
		if (!cellDimensionFlag){
			Warning::printMessage("Error in keyword "+keywords[Input::cellDimension]);
		}
		if (!originFlag){
			Warning::printMessage("Error in keyword "+keywords[Input::origin]);
		}
	}
	return (nCellsFlag && cellDimensionFlag && originFlag);
}

void MPM::setUpMaterialList(){

	json inputFile=input.getJson();
	map<Input::KeyWords,string> keywords = input.getKeyWords();

	// setup the material list
	if(inputFile.contains(keywords[Input::material]))
	{
		json::iterator it;

		for( it = inputFile[keywords[Input::material]].begin(); it!=inputFile[keywords[Input::material]].end();it++){
			
			if((*it)[keywords[Input::type]]==keywords[Input::elastic])
			{
				int id = (*it)[keywords[Input::id]];
				double young = (*it)[keywords[Input::young]];
				double poisson = (*it)[keywords[Input::poisson]];
				double density = (*it)[keywords[Input::density]];
				materials.push_back(new Elastic(id,density,young,poisson));
			}
		}
	}
	
	if (materials.empty())
	{
		Warning::printMessage("The material list can not be created. Verify the input file:");
		Warning::printMessage("Error in keyword "+keywords[Input::material]);
	}
}

void MPM::setUpBodyList(){

	json inputFile=input.getJson();
	map<Input::KeyWords,string> keywords = input.getKeyWords();

	if(inputFile.contains(keywords[Input::body]))
	{
		json::iterator it;

		for( it=inputFile[keywords[Input::body]].begin(); it!=inputFile[keywords[Input::body]].end();it++){
			
			if((*it)[keywords[Input::type]]==keywords[Input::cuboid])
			{
				int id = (*it)[keywords[Input::id]];
				Vector3d pointP1((*it)[keywords[Input::pointP1]][0],(*it)[keywords[Input::pointP1]][1],(*it)[keywords[Input::pointP1]][2]);
				Vector3d pointP2((*it)[keywords[Input::pointP2]][0],(*it)[keywords[Input::pointP2]][1],(*it)[keywords[Input::pointP2]][2]);
				int materialId = (*it)[keywords[Input::materialId]];

				BodyCuboid* iBody = new BodyCuboid();
				iBody->setId(id);
				iBody->setPoints(pointP1,pointP2);
				iBody->setMaterialId(materialId);
				bodies.push_back(iBody);
			}
		}
	}

	if (bodies.empty())
	{
		Warning::printMessage("The body list can not be created. Verify the input file:");
		Warning::printMessage("Error in keyword "+keywords[Input::body]);
	}
}

void MPM::createBodies(){

	for (size_t i = 0; i < bodies.size(); ++i)
	{	
		// get material from list
		Material* iMaterial=0;
		for (size_t i = 0; i < materials.size(); ++i) {

			if (bodies.at(i)->getMaterialId()==materials.at(i)->getId()){

				iMaterial=materials.at(i);
				break;
			}
		}
		bodies.at(i)->create(mesh,iMaterial);
	}
}

void MPM::setUpParticles(){

	for (size_t i = 0; i < bodies.size(); ++i)
	{	
		vector<Particle>& particles = bodies.at(i)->getParticles();

		for (size_t j = 0; j < particles.size(); ++j)
		{
			switch(ModelSetup::getInterpolationFunction())
			{
				case ModelSetup::LINEAR:
					particles.at(i).setShape(new ShapeLinear);
					break;

				case ModelSetup::GIMP:
					particles.at(i).setShape(new ShapeGimp);
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
