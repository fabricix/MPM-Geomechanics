/*
 * Input.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Input.h"
#include "SolverExplicitUSL.h"
#include "Elastic.h"
#include "BodyCuboid.h"
#include "Warning.h"

#include <fstream>
using std::ifstream;

namespace Input {

	json inputFile; //!< data structure containing all the model information
	string inputFileName; //!< file name to be read
}

inline const json& Input::getJson(){ return inputFile; }

inline string Input::getFileName(){ return inputFileName; }

void Input::readInputFile(string filename){

	// configures the input file	
	inputFileName = filename;

	if (inputFileName!=""){

		// read the file
		ifstream i(inputFileName);
		
		// initialize the json structure
		i >> inputFile;	
	}
}

bool Input::verifyData(json jsonObject, string keyword ) {

	return jsonObject.contains(keyword);
}

bool Input::verifyData(json jsonObject, string keyword, string dataType ) {

	// contain verification
	if (verifyData(jsonObject, keyword)){

		// data verification
		if (dataType=="array")
		{
			return jsonObject[keyword].is_array();
		}
		else if (dataType=="string")
		{
			return jsonObject[keyword].is_string();
		}
		else if (dataType=="number")
		{
			return jsonObject[keyword].is_number();
		}
		else if (dataType=="boolean")
		{
			return jsonObject[keyword].is_boolean();
		}
	}
	
	Warning::printMessage("Bad definition of keyword \""+keyword+"\" in the input file");
	throw (0);
	return false;
}

double Input::getSimulationTime(){
	
	if(verifyData(inputFile,"time","number")) {
		
		// return the simulation time
		return inputFile["time"];
	}
	return 0.0;
}

Solver* Input::getSolver() {

	if (verifyData(inputFile,"stress_scheme_update","string")) {

		if(inputFile["stress_scheme_update"]=="USL") {

			// return a USL solver
			return new SolverExplicitUSL();
		}

		Warning::printMessage("Bad definition of keyword \"stress_scheme_update\" in the input file");
		throw (0);
	}
	return 0;
}

ModelSetup::InterpolationFunctionType Input::getInterpolationFunction() {
	
	if (verifyData(inputFile,"shape_function","string")) {

		if(inputFile["shape_function"]=="GIMP") {

			// return GIMP shape function type
			return ModelSetup::GIMP;
		}

		if(inputFile["shape_function"]=="linear") {

			// return Linear shape function type
			return ModelSetup::LINEAR;
		}

		Warning::printMessage("Bad definition of keyword: \"shape_function\" in the input file");
		throw (0);
	}

	return ModelSetup::GIMP;
}

double Input::getTimeStep(){

	string keyword = "time_step";

	if (verifyData(inputFile, keyword, "number")){

		// return time step
		return inputFile[keyword];
	}

	return 0.0;
}

Vector3i Input::getCellsNum() {

	if(verifyData(inputFile,"mesh") && verifyData(inputFile["mesh"],"cells_number","array")){
		
		// return the number of cells
		Vector3i cellsNumber;
		cellsNumber(0)=inputFile["mesh"]["cells_number"][0];
		cellsNumber(1)=inputFile["mesh"]["cells_number"][1];
		cellsNumber(2)=inputFile["mesh"]["cells_number"][2];

		return cellsNumber;	
	}

	return Vector3i::Zero();
}

Vector3d Input::getCellDimension() {

	if(verifyData(inputFile,"mesh") && verifyData(inputFile["mesh"],"cells_dimension","array")){
		
		Vector3d cellDimension; 
		cellDimension(0)=inputFile["mesh"]["cells_dimension"][0];
		cellDimension(1)=inputFile["mesh"]["cells_dimension"][1];
		cellDimension(2)=inputFile["mesh"]["cells_dimension"][2];

		return cellDimension;
	}

	return Vector3d::Zero();
}

Vector3d Input::getOrigin() {

	if(verifyData(inputFile,"mesh") && verifyData(inputFile["mesh"],"origin", "array")){
			
		Vector3d originVector;
		originVector(0)=inputFile["mesh"]["origin"][0];
		originVector(1)=inputFile["mesh"]["origin"][1];
		originVector(2)=inputFile["mesh"]["origin"][2];

		return originVector;	
	}

	return Vector3d::Zero();
}

vector<Material*> Input::getMaterialList(){

	vector<Material*> materials;

	// setup the material list
	if (verifyData(inputFile, "material")){
	
		// loop aver all defined materials
		json::iterator it;
		for(it = inputFile["material"].begin(); it!=inputFile["material"].end();it++){
			
			// elastic material
			if (verifyData((*it),"type", "string") && (*it)["type"] == "elastic"){
				
				int id=0; if (verifyData((*it),"id","number")) { id = ((*it)["id"]); }
				double young=0.0; if (verifyData((*it),"young","number")) { young = ((*it)["young"]); }
				double poisson=0.0; if (verifyData((*it),"poisson","number")) { poisson = ((*it)["poisson"]); }
				double density=0.0; if (verifyData((*it),"density","number")) { density = ((*it)["density"]); }
				
				// create a new elastic material
				materials.push_back(new Elastic(id, density, young, poisson));
			}
		}
	}
	
	if (materials.empty()){

		Warning::printMessage("The material list was not created");
		throw (0);
	}

	return materials;
}

vector<Body*> Input::getBodyList(){

	vector<Body*> bodies;

	if (verifyData(inputFile, "body")){

		// loop aver all bodies
		json::iterator it;
		for(it=inputFile["body"].begin(); it!=inputFile["body"].end();it++){
			
			// cuboid body
			if (verifyData((*it),"type","string") && (*it)["type"] == "cuboid"){

				int id=0; if (verifyData((*it),"id","number")) { id = ((*it)["id"]); }

				Vector3d pointP1=Vector3d::Zero();
				if (verifyData((*it), "point_p1", "array")){

					pointP1(0)=(*it)["point_p1"][0];
					pointP1(1)=(*it)["point_p1"][1];
					pointP1(2)=(*it)["point_p1"][2];
				}
				
				Vector3d pointP2=Vector3d::Zero();
				if (verifyData((*it),"point_p2","array")){

					pointP2(0)=(*it)["point_p2"][0];
					pointP2(1)=(*it)["point_p2"][1];
					pointP2(2)=(*it)["point_p2"][2];
				}

				int material_id=0; if (verifyData((*it),"material_id","number")) { material_id = ((*it)["material_id"]); }

				// create a new cuboid
				BodyCuboid* iBody = new BodyCuboid();
				iBody->setId(id);
				iBody->setPoints(pointP1,pointP2);
				iBody->setMaterialId(material_id);

				bodies.push_back(iBody);
			}
		}
	}

	if (bodies.empty()){

		Warning::printMessage("The body list was not created");
		throw (0);
	}

	return bodies;
}

Vector3d Input::getGravity(){

	Vector3d gravity=Vector3d::Zero();
	
	if (verifyData(inputFile, "gravity", "array")){

		gravity.x()=inputFile["gravity"][0];
		gravity.y()=inputFile["gravity"][1];
		gravity.z()=inputFile["gravity"][2];
	}
	
	return gravity;
}

int Input::getResultNum(){

	int results = 10;

	if (verifyData(inputFile, "results") && verifyData(inputFile["results"],"print","number")) {
			
			results = inputFile["results"]["print"];
	}

	return results;
}

vector<string> Input::getResultFields() {

	vector<string> fields;

	if (verifyData(inputFile,"results") && verifyData(inputFile["results"],"fields","array")){

		json::iterator it;
		for( it=inputFile["results"]["fields"].begin(); 
			it!=inputFile["results"]["fields"].end();it++)
		{
			if ((*it).is_string()){

				fields.push_back(*it);
			}
		}
	}
	
	if (fields.empty()){

		Warning::printMessage("The result fields list was not created");
		throw (0);
	}
	
	return fields;
}

ModelSetup::DampingType Input::getDampingType() {

	if (verifyData(inputFile,"damping"))
	{
		if(verifyData(inputFile["damping"],"type","string")){

			// local damping
			if (inputFile["damping"]["type"]=="local") {

				return ModelSetup::DampingType::LOCAL;
			}
		}
		Warning::printMessage("Bad definition of damping type");
		throw (0);
	}
	
	return ModelSetup::DampingType::UNDAMPED;
}

double Input::getDampingValue() {

	if (verifyData(inputFile,"damping")&&verifyData(inputFile["damping"],"type","string"))
	{
		if (inputFile["damping"]["type"]=="local" && verifyData(inputFile["damping"],"value","number"))
		{
			return inputFile["damping"]["value"];
		}
	}
	
	Warning::printMessage("Bad definition of damping value");
	throw (0);
	
	return 0.0;
}