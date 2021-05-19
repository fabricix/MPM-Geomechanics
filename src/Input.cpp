/*
 * Input.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Input.h"
#include "SolverUSL.h"
#include "Elastic.h"
#include "BodyCuboid.h"
#include "Warning.h"

#include <fstream>
using std::ifstream;

Input::Input() {

		keywords[KeyWords::alpha]="alpha";
		keywords[KeyWords::body]="body";
		keywords[KeyWords::boundaryConditons]="boundaryConditons";
		keywords[KeyWords::cellDimension]="cellDimension";
		keywords[KeyWords::cohesion]="cohesion";
		keywords[KeyWords::cuboid]="cuboid";
		keywords[KeyWords::damping]="damping";
		keywords[KeyWords::density]="density";
		keywords[KeyWords::displacement]="displacement";
		keywords[KeyWords::elastic]="elastic";
		keywords[KeyWords::fields]="fields";
		keywords[KeyWords::fixed]="fixed";
		keywords[KeyWords::GIMP]="GIMP";
		keywords[KeyWords::friction]="friction";
		keywords[KeyWords::id]="id";
		keywords[KeyWords::localNoViscous]="localNoViscous";
		keywords[KeyWords::linear]="linear";
		keywords[KeyWords::mass]="mass";
		keywords[KeyWords::materialId]="materialId";
		keywords[KeyWords::material]="material";
		keywords[KeyWords::mesh]="mesh";
		keywords[KeyWords::mesh]="mesh";
		keywords[KeyWords::nCells]="nCells";
		keywords[KeyWords::nThreads]="nThreads";
		keywords[KeyWords::number]="number";
		keywords[KeyWords::origin]="origin";
		keywords[KeyWords::paneX0]="paneX0";
		keywords[KeyWords::paneXn]="paneXn";
		keywords[KeyWords::paneY0]="paneY0";
		keywords[KeyWords::paneYn]="paneYn";
		keywords[KeyWords::paneZ0]="paneZ0";
		keywords[KeyWords::paneZn]="paneZn";
		keywords[KeyWords::particle]="particle";
		keywords[KeyWords::plastic]="plastic";
		keywords[KeyWords::pointP1]="pointP1";
		keywords[KeyWords::pointP2]="pointP2";
		keywords[KeyWords::poisson]="poisson";
		keywords[KeyWords::position]="position";
		keywords[KeyWords::results]="results";
		keywords[KeyWords::shapeFunction]="shapeFunction";
		keywords[KeyWords::sliding]="sliding";
		keywords[KeyWords::stress]="stress";
		keywords[KeyWords::stressSchemeUpdate]="stressSchemeUpdate";
		keywords[KeyWords::structured]="structured";
		keywords[KeyWords::time]="time";
		keywords[KeyWords::timeStep]="timeStep";
		keywords[KeyWords::timeStepFraction]="timeStepFraction";
		keywords[KeyWords::type]="type";
		keywords[KeyWords::USL]="USL";
		keywords[KeyWords::young]="young";
}

Input::~Input() {
	// TODO Auto-generated destructor stub
}

map<Input::KeyWords,string> Input::getKeyWords(){

	return keywords;
}

json Input::getJson(){

	return inputFile;
}

string Input::getFileName(){

	return inputFileName;
}

void Input::setFileName(string ifn){

	inputFileName=ifn;
}

void Input::readInputFile(string filename){

	setFileName(filename);
	readInputFile();
}

void Input::readInputFile()
{
	if (inputFileName!="")
	{
		// read the file
		ifstream i(inputFileName);
		
		// initialize the json structure
		i >> inputFile;
	}
}

double Input::getSimulationTime(){

	if(inputFile.contains(keywords[Input::time])&&inputFile[keywords[Input::time]].is_number())
	{
		return inputFile[keywords[Input::time]];
	}
	else
	{
		Warning::printMessage("Verify "+keywords[Input::time]+" definition in the input file");
	}
	return 0.0;
}

Solver* Input::getSolver(){
	
	if(inputFile.contains(keywords[Input::stressSchemeUpdate]))
	{
		if(inputFile[keywords[Input::stressSchemeUpdate]]==keywords[Input::USL]) {
			
			return new SolverUSL();
		}
	}
	
	Warning::printMessage("Verify "+keywords[Input::stressSchemeUpdate]+" definition in the input file");
	return 0;
}

ModelSetup::InterpolationFunctionType Input::getInterpolationFunction(){
	
	if(inputFile.contains(keywords[Input::shapeFunction]))
	{
		if(inputFile[keywords[Input::shapeFunction]]==keywords[Input::GIMP]) {

			return ModelSetup::GIMP;
		}

		if(inputFile[keywords[Input::shapeFunction]]==keywords[Input::linear]) {
			return ModelSetup::LINEAR;
		}
	}
	
	Warning::printMessage("Verify "+keywords[Input::shapeFunction]+" definition in the input file");
	return ModelSetup::GIMP;
}

double Input::getTimeStep(){

	if(inputFile.contains(keywords[Input::timeStep])&&inputFile[keywords[Input::timeStep]].is_number()){

		return inputFile[keywords[Input::timeStep]];
	}

	Warning::printMessage("Verify "+keywords[Input::timeStep]+" definition in the input file");
	return 0.0;
}

Vector3i Input::getCellsNum(){

	Vector3i nCells; 

	if(inputFile.contains(keywords[Input::mesh])&&inputFile[keywords[Input::mesh]].contains(keywords[Input::nCells])){	
		
		if(inputFile[keywords[Input::mesh]][keywords[Input::nCells]].is_array()) {

			nCells(0)=inputFile[keywords[Input::mesh]][keywords[Input::nCells]][0];
			nCells(1)=inputFile[keywords[Input::mesh]][keywords[Input::nCells]][1];
			nCells(2)=inputFile[keywords[Input::mesh]][keywords[Input::nCells]][2];
			return nCells;
		}
	}

	Warning::printMessage("Error in keyword "+keywords[Input::nCells]);
	return Vector3i::Zero();
}

Vector3d Input::getCellDimension(){

	Vector3d cellDimension; 

	if(inputFile.contains(keywords[Input::mesh])&&inputFile[keywords[Input::mesh]].contains(keywords[Input::cellDimension])){

		if(inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]].is_array()) {
		
				cellDimension(0)=inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]][0];
				cellDimension(1)=inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]][1];
				cellDimension(2)=inputFile[keywords[Input::mesh]][keywords[Input::cellDimension]][2];
				
				return cellDimension;
		}
	}

	Warning::printMessage("Error in keyword "+keywords[Input::cellDimension]);
	return Vector3d::Zero();
}

Vector3d Input::getOrigin(){

	Vector3d origin;

	if(inputFile.contains(keywords[Input::mesh])&&inputFile[keywords[Input::mesh]].contains(keywords[Input::origin])){

		if(inputFile[keywords[Input::mesh]][keywords[Input::origin]].is_array()) {
			
				origin(0)=inputFile[keywords[Input::mesh]][keywords[Input::origin]][0];
				origin(1)=inputFile[keywords[Input::mesh]][keywords[Input::origin]][1];
				origin(2)=inputFile[keywords[Input::mesh]][keywords[Input::origin]][2];
				
				return origin;
		}	
	}

	Warning::printMessage("Error in keyword "+keywords[Input::origin]);
	return Vector3d::Zero();
}

vector<Material*> Input::getMaterialList(){

	vector<Material*> materials;

	// setup the material list
	if(inputFile.contains(keywords[Input::material]))
	{
		json::iterator it;

		for(it = inputFile[keywords[Input::material]].begin(); it!=inputFile[keywords[Input::material]].end();it++){
			
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
	
	if (materials.empty()){

		Warning::printMessage("The material list can not be created. Verify the input file:");
	}

	return materials;
}


vector<Body*> Input::getBodyList(){

	vector<Body*> bodies;

	if(inputFile.contains(keywords[Input::body]))
	{
		json::iterator it;

		for(it=inputFile[keywords[Input::body]].begin(); it!=inputFile[keywords[Input::body]].end();it++){
			
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

	return bodies;
}
