/*
 * Input.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Input.h"
#include "Solver/SolverExplicitUSL.h"
#include "Materials/Elastic.h"
#include "Materials/MohrCoulomb.h"
#include "Body/BodyCuboid.h"
#include "Body/BodyPolygon.h"
#include "Warning.h"

#include <omp.h>

#include <limits>
using std::numeric_limits;

#include <fstream>
using std::ifstream;

#include<string>
using std::string;
using std::to_string;

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
				
				// verify material type
				if (!(*it)["type"].is_null() && (*it)["type"].is_string()){
					
					// elastic material
					if ((*it)["type"] == "elastic")
					{
						// material properties
						int id=0; if ((*it)["id"].is_number()) { id = ((*it)["id"]); }
						double young=0.0; if ((*it)["young"].is_number()) { young = ((*it)["young"]); }
						double poisson=0.0; if ((*it)["poisson"].is_number()) { poisson = ((*it)["poisson"]); }
						double density=0.0; if ((*it)["density"].is_number()) { density = ((*it)["density"]); }
						
						// create a new elastic material
						materials.push_back(new Elastic(id, density, young, poisson));
					}
					// mohr-coulomb material
					else if ((*it)["type"] == "mohr-coulomb")
					{	
						// material properties
						int id=0; if ((*it)["id"].is_number()) { id = ((*it)["id"]); }
						double young=0.0; if ((*it)["young"].is_number()) { young = ((*it)["young"]); }
						double poisson=0.0; if ((*it)["poisson"].is_number()) { poisson = ((*it)["poisson"]); }
						double density=0.0; if ((*it)["density"].is_number()) { density = ((*it)["density"]); }
						double friction=0.0; if ((*it)["friction"].is_number()) { friction = ((*it)["friction"]); }
						double cohesion=0.0; if ((*it)["cohesion"].is_number()) { cohesion = ((*it)["cohesion"]); }
						double dilation=0.0; if ((*it)["dilation"].is_number()) { dilation = ((*it)["dilation"]); }
						double tensile=numeric_limits<double>::max(); if ((*it)["tensile"].is_number()) { tensile = ((*it)["tensile"]); }
						
						// create a new material
						materials.push_back(new MohrCoulomb(id, density, young, poisson, friction, cohesion, dilation, tensile));	
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

				if ((*it)["type"] == "polygon_2d") {

					// body id
					int id=0; 
					if ((*it)["id"].is_number()){
						id = ((*it)["id"]);
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

					// extrude direction
					string extrude_direction=""; 
					if ((*it)["extrude_direction"].is_string()) 
					{
					 	extrude_direction = ((*it)["extrude_direction"]);
					}
					else
						throw(0);

					// extrude displacement
					double extrude_displacement=0; 
					if ((*it)["extrude_displacement"].is_number()) 
					{
					 	extrude_displacement = ((*it)["extrude_displacement"]);
					}
					else
						throw(0);

					// discretization length
					double discretization_length=0; 
					if ((*it)["discretization_length"].is_number()) 
					{
					 	discretization_length = ((*it)["discretization_length"]);
					}
					else
						throw(0);

					// points
					vector<Vector3d> polygon_points;
					if ((*it)["points"].is_array()) 
					{
					 	for (size_t i = 0; i < (*it)["points"].size(); ++i)
					 	{
					 		// point position
					 		double px = (*it)["points"].at(i).at(0);
					 		double py = (*it)["points"].at(i).at(1);
					 		double pz = (*it)["points"].at(i).at(2);

					 		polygon_points.push_back(Vector3d(px,py,pz));
					 	}
					 	
					 	if (polygon_points.size()!=0)
					 	{
					 		// create a new polygon body
					 		BodyPolygon* iBody = new BodyPolygon();

							if (iBody==NULL)
							{
								throw(0);
							}
							else
							{
								iBody->setId(id);
								iBody->setPoints(polygon_points);
								iBody->setMaterialId(material_id);
								iBody->setExtrudeDirection(extrude_direction);
								iBody->setExtrudeDisplacement(extrude_displacement);
								iBody->setDiscretizationLength(discretization_length);
							}
						
							bodies.push_back(iBody);
					 	}
					 	else
					 		throw(0);
					}
					else
						throw(0);
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

vector<Boundary::BoundaryType> Input::getMeshBoundaryConditionsFluid() {

	try{
		// initialize the vector as sliding restriction
		vector<Boundary::BoundaryType> restrictions(6,Boundary::BoundaryType::SLIDING);

		// if not defined set all sliding boundaries
		if(inputFile["mesh"]["boundary_conditions_fluid"].is_null()) {
			return 	restrictions;
		}

		// Planes X0, Y0 and Z0
		setRestriction(0,restrictions,inputFile["mesh"]["boundary_conditions_fluid"]["plane_X0"]);
		setRestriction(1,restrictions,inputFile["mesh"]["boundary_conditions_fluid"]["plane_Y0"]);
		setRestriction(2,restrictions,inputFile["mesh"]["boundary_conditions_fluid"]["plane_Z0"]);

		// Planes Xn, Yn and Zn
		setRestriction(3,restrictions,inputFile["mesh"]["boundary_conditions_fluid"]["plane_Xn"]);
		setRestriction(4,restrictions,inputFile["mesh"]["boundary_conditions_fluid"]["plane_Yn"]);
		setRestriction(5,restrictions,inputFile["mesh"]["boundary_conditions_fluid"]["plane_Zn"]);

		return restrictions;
	}
	catch (...) {
		Warning::printMessage("Error during reading the boundary conditions in fluid");
		throw;
	}
}

unsigned Input::getNumThreads() {

	try
	{
		if(inputFile["n_threads"].is_null() || !inputFile["n_threads"].is_number()) {
			
			#ifdef _OPENMP
			return omp_get_num_procs();
			#endif

			return 	1;
		}
		else
		{
			return inputFile["n_threads"];
		}
	}
	catch(...)
	{
		Warning::printMessage("Error during reading the number of threads");
		throw;
	}
}


unsigned Input::getNumberPhases(){

	try
	{
		string keyword = "n_phases";

		// default simulations is one phase
		if(inputFile[keyword].is_null()) { return 1; }

		// define the number of phases by an input
		if(inputFile[keyword].is_number()) {
			
			unsigned nPhases = inputFile[keyword];

			if (nPhases>0 && nPhases<=2){

				return nPhases; 	
			}
			else{

				throw(nPhases);
			}
		}

		throw(keyword);
	}

	catch(std::string& keyword)
	{
		Warning::printMessage("The keyword: \""+string(keyword)+"\" must be a number");
		throw;
	}

	catch(unsigned nPhases)
	{
		Warning::printMessage("The number of phases must be 1 or 2 (input = "+to_string(nPhases)+")");
		throw;
	}
}
