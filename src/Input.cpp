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

inline const json& Input::getJson() {

	return inputFile; 
}

inline string Input::getFileName() { 

	return inputFileName; 
}

void Input::readInputFile(string filename) {


	try{
		// configures the input file	
		inputFileName = filename;
		
		if (inputFileName!=""){

			// read the file
			ifstream i(inputFileName);
			
			// initialize the json structure
			i >> inputFile;	
		}
		else
			throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error in input file name");
		throw;
	}
}

double Input::getSimulationTime(){
	
	try{
		if(inputFile["time"].is_number()) {

			return inputFile["time"];
		}
		else
			throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error in input file name");
		throw;
	}
}

Solver* Input::getSolver() {
	
	try
	{	
		// default value
		if (inputFile["stress_scheme_update"].is_null())
		{
			return new SolverExplicitUSL();			
		}

		if (inputFile["stress_scheme_update"].is_string()) {

			// USL scheme
			if(inputFile["stress_scheme_update"]=="USL") {

				return new SolverExplicitUSL();
			}

			throw (0);
		}
		
		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error in solver definition in input file");
		throw;
	}
}

ModelSetup::InterpolationFunctionType Input::getInterpolationFunction() {
	
	try
	{	
		// default value
		if (inputFile["shape_function"].is_null())
		{
			return ModelSetup::GIMP;	
		}

		if (inputFile["shape_function"].is_string()) {

			if(inputFile["shape_function"]=="GIMP") {

				// return GIMP shape function type
				return ModelSetup::GIMP;
			}

			if(inputFile["shape_function"]=="linear") {

				// return Linear shape function type
				return ModelSetup::LINEAR;
			}

			throw(0);
		}

		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Bad definition of shape function in input file");
		throw;
	}
}

double Input::getTimeStep(){

	try
	{
		if (inputFile["time_step"].is_number()){

			// return time step
			return inputFile["time_step"];
		}
		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Bad definition of time step in input file");
		throw;
	}
}

Vector3i Input::getCellsNum() {

	try
	{
		if(inputFile["mesh"]["cells_number"].is_array()){
			
			// return the number of cells
			Vector3i cellsNumber;
			cellsNumber(0)=inputFile["mesh"]["cells_number"][0];
			cellsNumber(1)=inputFile["mesh"]["cells_number"][1];
			cellsNumber(2)=inputFile["mesh"]["cells_number"][2];

			return cellsNumber;	
		}

		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Bad definition of cell number in input file");
		throw;
	}
}

Vector3d Input::getCellDimension() {

	try
	{	
		if(inputFile["mesh"]["cells_dimension"].is_array()){
			
			Vector3d cellDimension; 
			cellDimension(0)=inputFile["mesh"]["cells_dimension"][0];
			cellDimension(1)=inputFile["mesh"]["cells_dimension"][1];
			cellDimension(2)=inputFile["mesh"]["cells_dimension"][2];

			return cellDimension;
		}

		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Bad definition of cell dimension in input file");
		throw;
	}
}

Vector3d Input::getOrigin() {

	try
	{
		if(inputFile["mesh"]["origin"].is_array()){
				
			Vector3d originVector;
			originVector(0)=inputFile["mesh"]["origin"][0];
			originVector(1)=inputFile["mesh"]["origin"][1];
			originVector(2)=inputFile["mesh"]["origin"][2];

			return originVector;	
		}
	
		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error reading mesh origin");
		throw;
	}
}

vector<Material*> Input::getMaterialList(){

	try{

		vector<Material*> materials;

		// setup the material list
		if (!inputFile["material"].is_null()){
		
			// loop aver all defined materials
			json::iterator it;
			for(it = inputFile["material"].begin(); it!=inputFile["material"].end();it++){
				
				// elastic material
				if (!(*it)["type"].is_null() && (*it)["type"].is_string()){
					
					if ((*it)["type"] == "elastic")
					{
						int id=0; 
						if ((*it)["id"].is_number()) { id = ((*it)["id"]); }
						double young=0.0; if ((*it)["young"].is_number()) { young = ((*it)["young"]); }
						double poisson=0.0; if ((*it)["poisson"].is_number()) { poisson = ((*it)["poisson"]); }
						double density=0.0; if ((*it)["density"].is_number()) { density = ((*it)["density"]); }
						
						// create a new elastic material
						materials.push_back(new Elastic(id, density, young, poisson));
					}
				}
			}
		}

		if (materials.empty())
		{
			throw(0);
		}
		return materials;
	}
	catch(...)
	{
		Warning::printMessage("Error in materials definition in input file");
		throw;
	}
}

vector<Body*> Input::getBodyList(){

	vector<Body*> bodies;
	try
	{
		if (!inputFile["body"].is_null()){

			// loop aver all bodies
			json::iterator it;
			for(it=inputFile["body"].begin(); it!=inputFile["body"].end();it++) {
				
				// cuboid body
				if ((*it)["type"] == "cuboid") {

					// body id
					int id=0; 
					if ((*it)["id"].is_number()){
						id = ((*it)["id"]);
					}
					else
						throw(0);

					// point P1
					Vector3d pointP1=Vector3d::Zero();
					if ((*it)["point_p1"].is_array())
					{
						pointP1(0)=(*it)["point_p1"][0];
						pointP1(1)=(*it)["point_p1"][1];
						pointP1(2)=(*it)["point_p1"][2];
					}
					else
						throw(0);
					
					// point P2
					Vector3d pointP2=Vector3d::Zero();
					if ((*it)["point_p2"].is_array())
					{
						pointP2(0)=(*it)["point_p2"][0];
						pointP2(1)=(*it)["point_p2"][1];
						pointP2(2)=(*it)["point_p2"][2];
					}
					else
						throw(0);

					// material id
					int material_id=0; 
					if ((*it)["material_id"].is_number()) 
					{
					 	material_id = ((*it)["material_id"]);
					}
					else
						throw(0);

					// create a new cuboid
					BodyCuboid* iBody = new BodyCuboid();

					if (iBody==NULL)
					{
						throw(0);
					}
					else
					{
						iBody->setId(id);
						iBody->setPoints(pointP1,pointP2);
						iBody->setMaterialId(material_id);
					}

					bodies.push_back(iBody);
				}
			}
		}

		if (bodies.empty()){

			throw(0);
		}

		return bodies;
	}
	catch(...)
	{
		Warning::printMessage("Error in body definition in input file");
		throw;
	}
}

Vector3d Input::getGravity(){

	try
	{
		Vector3d gravity=Vector3d::Zero();
		
		// default value
		if (inputFile["gravity"].is_null())
		{
			return gravity;
		}

		if (inputFile["gravity"].is_array())
		{
			gravity.x()=inputFile["gravity"][0];
			gravity.y()=inputFile["gravity"][1];
			gravity.z()=inputFile["gravity"][2];
			return gravity;
		}
		
		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error during reading gravity");
		throw;
	}
}

int Input::getResultNum(){

	try
	{
		int results = 10;

		if (inputFile["results"]["print"].is_null())
		{
			return results;
		}		

		if (inputFile["results"]["print"].is_number())
		{
			return inputFile["results"]["print"];
		}
		
		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error during reading the number of results");
		throw;
	}
}

vector<string> Input::getResultFields() {

	try
	{
		vector<string> fields;
		
		if (inputFile["results"].is_null())
		{
			fields.push_back("displacement");
			return fields;
		}

		if (inputFile["results"]["fields"].is_null())
		{
			throw(0);
		}

		// get all results fields
		json::iterator it;
		for( it=inputFile["results"]["fields"].begin();it!=inputFile["results"]["fields"].end();it++){

			if ((*it).is_string()){
				fields.push_back(*it);
			}
		}

		if (fields.empty()){

			throw (0);
		}

		return fields;
	}
	catch(...)
	{
		Warning::printMessage("Error during the field results creation");
		throw;
	}
}

ModelSetup::DampingType Input::getDampingType() {

	try
	{
		if (inputFile["damping"].is_null())
			return ModelSetup::DampingType::UNDAMPED;

		if (inputFile["damping"]["type"].is_null())
			throw(0);
		
		if(inputFile["damping"]["type"]=="local")
			return ModelSetup::DampingType::LOCAL;

		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error in damping keyword");
		throw;
	}
}

double Input::getDampingValue() {

	try
	{
		if (inputFile["damping"].is_null()){

			return 0.0;
		}

		if (inputFile["damping"]["type"].is_string())
		{
			if (inputFile["damping"]["type"]=="local" && inputFile["damping"]["value"].is_number())
			{
				return inputFile["damping"]["value"];
			}

			throw (0);
		}

		throw (0);
	}
	catch(...)
	{
		Warning::printMessage("Bad definition of damping value");
		throw;
	}
}

static void setRestriction(size_t index,vector<Boundary::BoundaryType>& restrictions, string resPlane ) {

	if (resPlane=="fixed")
	{
		restrictions.at(index)=Boundary::BoundaryType::FIXED;
	}
	else if (resPlane=="free")
	{
		restrictions.at(index)=Boundary::BoundaryType::FREE;
	}
	else if (resPlane=="sliding")
	{
		restrictions.at(index)=Boundary::BoundaryType::SLIDING;
	}
	else
	{
		throw(0);
	}
}

vector<Boundary::BoundaryType> Input::getMeshBoundaryConditions() {

	try{
		// initialize the vector as sliding restriction
		vector<Boundary::BoundaryType> restrictions(6,Boundary::BoundaryType::SLIDING);

		// if not defined set all sliding boundaries
		if(inputFile["mesh"]["boundary_conditions"].is_null()) {
			return 	restrictions;
		}

		// Planes X0, Y0 and Z0
		setRestriction(0,restrictions,inputFile["mesh"]["boundary_conditions"]["plane_X0"]);
		setRestriction(1,restrictions,inputFile["mesh"]["boundary_conditions"]["plane_Y0"]);
		setRestriction(2,restrictions,inputFile["mesh"]["boundary_conditions"]["plane_Z0"]);

		// Planes Xn, Yn and Zn
		setRestriction(3,restrictions,inputFile["mesh"]["boundary_conditions"]["plane_Xn"]);
		setRestriction(4,restrictions,inputFile["mesh"]["boundary_conditions"]["plane_Yn"]);
		setRestriction(5,restrictions,inputFile["mesh"]["boundary_conditions"]["plane_Zn"]);

		return restrictions;
	}
	catch (...) {
		Warning::printMessage("Error during reading the boundary conditions");
		throw;
	}
}
