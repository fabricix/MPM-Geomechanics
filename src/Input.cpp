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

namespace Input {

		map<Input::KeyWords,string> keywords; //!< keyword the access to the data structure
		json inputFile; //!< data structure containing all the model informations
		string inputFileName; //!< file name to be read
}

void Input::initKeyWords(){

		keywords[Input::KeyWords::alpha]="alpha";
		keywords[Input::KeyWords::body]="body";
		keywords[Input::KeyWords::boundaryConditons]="boundaryConditons";
		keywords[Input::KeyWords::cellDimension]="cellDimension";
		keywords[Input::KeyWords::cohesion]="cohesion";
		keywords[Input::KeyWords::cuboid]="cuboid";
		keywords[Input::KeyWords::damping]="damping";
		keywords[Input::KeyWords::density]="density";
		keywords[Input::KeyWords::displacement]="displacement";
		keywords[Input::KeyWords::elastic]="elastic";
		keywords[Input::KeyWords::fields]="fields";
		keywords[Input::KeyWords::fixed]="fixed";
		keywords[Input::KeyWords::gravity]="gravity";
		keywords[Input::KeyWords::GIMP]="GIMP";
		keywords[Input::KeyWords::friction]="friction";
		keywords[Input::KeyWords::id]="id";
		keywords[Input::KeyWords::localNoViscous]="localNoViscous";
		keywords[Input::KeyWords::linear]="linear";
		keywords[Input::KeyWords::mass]="mass";
		keywords[Input::KeyWords::materialId]="materialId";
		keywords[Input::KeyWords::material]="material";
		keywords[Input::KeyWords::mesh]="mesh";
		keywords[Input::KeyWords::mesh]="mesh";
		keywords[Input::KeyWords::nCells]="nCells";
		keywords[Input::KeyWords::nThreads]="nThreads";
		keywords[Input::KeyWords::number]="number";
		keywords[Input::KeyWords::origin]="origin";
		keywords[Input::KeyWords::paneX0]="paneX0";
		keywords[Input::KeyWords::paneXn]="paneXn";
		keywords[Input::KeyWords::paneY0]="paneY0";
		keywords[Input::KeyWords::paneYn]="paneYn";
		keywords[Input::KeyWords::paneZ0]="paneZ0";
		keywords[Input::KeyWords::paneZn]="paneZn";
		keywords[Input::KeyWords::particle]="particle";
		keywords[Input::KeyWords::plastic]="plastic";
		keywords[Input::KeyWords::pointP1]="pointP1";
		keywords[Input::KeyWords::pointP2]="pointP2";
		keywords[Input::KeyWords::poisson]="poisson";
		keywords[Input::KeyWords::position]="position";
		keywords[Input::KeyWords::results]="results";
		keywords[Input::KeyWords::shapeFunction]="shapeFunction";
		keywords[Input::KeyWords::sliding]="sliding";
		keywords[Input::KeyWords::stress]="stress";
		keywords[Input::KeyWords::stressSchemeUpdate]="stressSchemeUpdate";
		keywords[Input::KeyWords::structured]="structured";
		keywords[Input::KeyWords::time]="time";
		keywords[Input::KeyWords::timeStep]="timeStep";
		keywords[Input::KeyWords::timeStepFraction]="timeStepFraction";
		keywords[Input::KeyWords::type]="type";
		keywords[Input::KeyWords::USL]="USL";
		keywords[Input::KeyWords::young]="young";
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
	
	initKeyWords();
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

Vector3d Input::getGravity(){

	Vector3d gravity=Vector3d::Zero();
	
	if(inputFile.contains(keywords[Input::gravity])&&inputFile[keywords[Input::gravity]].is_array()){

		gravity.x()=inputFile[keywords[Input::gravity]][0];
		gravity.y()=inputFile[keywords[Input::gravity]][1];
		gravity.z()=inputFile[keywords[Input::gravity]][2];
	}
	
	return gravity;
}


int Input::getResultNum(){

	int results = 10;
	if(inputFile.contains(keywords[Input::results])&&inputFile[keywords[Input::results]].is_number()){
		
		results=inputFile[keywords[Input::results]];
	}
	return results;
}
