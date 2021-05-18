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
		keywords[KeyWords::friction]="friction";
		keywords[KeyWords::id]="id";
		keywords[KeyWords::localNoViscous]="localNoViscous";
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