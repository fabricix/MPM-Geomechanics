/*
 * Input.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Input.h"
#include "Solver/SolverExplicitUSL.h"
#include "Solver/SolverExplicitTwoPhaseUSL.h"
#include "Materials/Elastic.h"
#include "Materials/MohrCoulomb.h"
#include "Body/BodyCuboid.h"
#include "Body/BodyPolygon.h"
#include "Body/BodyParticle.h"
#include "Body/BodySphere.h"
#include "Particle/Particle.h"
#include "Particle/ParticleMixture.h"
#include "Warning.h"
#include "Loads.h"

#include <omp.h>

#include <limits>
using std::numeric_limits;

#include <fstream>
using std::ifstream;

#include<string>
using std::string;
using std::to_string;

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace Loads;


namespace Input {

	json inputFile; //!< data structure containing all the model information
	string inputFileName; //!< file name to be read

	using json = nlohmann::json;

	template<typename T>
	T get_number(const json& j, const std::string& key, T default_value = T()) {
		if (j.contains(key) && j[key].is_number()) {
			return j[key].get<T>();
		}
		return default_value;
	}

	bool get_boolean(const json& j, const std::string& key, bool default_value = false) {
		if (j.contains(key) && j[key].is_boolean()) {
			return j[key].get<bool>();
		}
		return default_value;
	}

	std::string get_string(const json& j, const std::string& key, const std::string& default_value = "") {
		if (j.contains(key) && j[key].is_string()) {
			return j[key].get<std::string>();
		}
		return default_value;
	}
}

inline const json& Input::getJson( ) { return inputFile; }

inline string Input::getFileName( ) { return inputFileName; }

bool Input::getLoadState() { return Input::get_boolean(Input::getJson(), "load_state", false); };

bool Input::getSaveState() { return Input::get_boolean(Input::getJson(), "save_state", false); };

void Input::readInputFile(string filename) {

	try{
		// configures the input file	
		inputFileName = filename;
		
		if ( inputFileName != "" ) {

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
		Warning::printMessage("Was not possible read the file,\nplease check the input file name...");
		throw;
	}
}

double Input::getSimulationTime(){
	
	try{
		double time = get_number(inputFile,"time",0.0);
		
		if(time>0) 
		{ 
			return time; 
		}
		else
			throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Simulation time must be greater than zero");
		throw;
	}
}

Solver* Input::getSolver() {
	
	try
	{	
		string key = "stress_scheme_update";
		
		// default value
		if (inputFile[key].is_null()) {

			if (ModelSetup::getTwoPhaseActive()) { 

				return new SolverExplicitTwoPhaseUSL(); 
			}

			return new SolverExplicitUSL();
		}

		if (inputFile[key].is_string()) {

			// USL scheme
			if(inputFile[key]=="USL") {

				if (ModelSetup::getTwoPhaseActive()) { 

					return new SolverExplicitTwoPhaseUSL(); 
				}

				return new SolverExplicitUSL();
			}
			throw (key);
		}
		throw(0);
	}

	catch(string& key)
	{
		Warning::printMessage("Error in keyword: "+key);
		throw;
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
		if (inputFile["time_step"].is_null() || !inputFile["time_step"].is_number() )
		{ 
			return 0;
		}
		else
		{
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

double Input::getCriticalTimeStepMultiplier()
{
	try
	{
		string key = "critical_time_step_multiplier";
		
		if (!inputFile[key].is_null() && inputFile[key].is_number())
		{
			return inputFile[key];
		}
		else
		{
			return 0;
		}
	}
	catch(...)
	{
		Warning::printMessage("Bad definition of time step multiplier");
		throw;
	}	
}

Vector3i Input::getCellsNum()
{
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

	// Setup the material list
	if (!inputFile["material"].is_null() || !inputFile["materials"].is_null()) {
		
	// Determine the correct key
	auto materialKey = inputFile["material"].is_null() ? "materials" : "material";

    // Loop over all defined materials
    json::iterator it;
    for (it = inputFile[materialKey].begin(); it != inputFile[materialKey].end(); it++) {
	
				// verify material type
				if (!(*it)["type"].is_null() && (*it)["type"].is_string()){
					
					Material* material = NULL;

					// elastic material
					if ((*it)["type"] == "elastic")
					{
						// material properties
						int id=0; if ((*it)["id"].is_number()) { id = ((*it)["id"]); }
						double young=0.0; if ((*it)["young"].is_number()) { young = ((*it)["young"]); }
						double poisson=0.0; if ((*it)["poisson"].is_number()) { poisson = ((*it)["poisson"]); }
						double density=0.0; if ((*it)["density"].is_number()) { density = ((*it)["density"]); }
						
						// create a new elastic material
						material = new Elastic(id, density, young, poisson);
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
						double tensile = 0.0; if ((*it)["tensile"].is_number()) { tensile = ((*it)["tensile"]); }

						// create a new softening object and configure it
						MohrCoulomb::Softening softening;
						if ((*it).contains("softening") && (*it)["softening"]=="exponential")
						{
							softening.softening_type = MohrCoulomb::Softening::SofteningType::EXPONENTIAL;
							softening.exponential_shape_factor = get_number((*it),"softening.exponential.eta",0);
							softening.friction_residual = get_number((*it),"softening.friction.residual",friction);
							softening.cohesion_residual = get_number((*it),"softening.cohesion.residual",cohesion); 
							softening.tensile_residual = get_number((*it),"softening.tensile.residual",tensile);

							softening.friction_softening_active = get_boolean((*it),"softening.friction.active",false);
							softening.cohesion_softening_active = get_boolean((*it),"softening.cohesion.active",false);
							softening.tensile_softening_active = get_boolean((*it),"softening.tensile.active",false);
						}
						
						// create a new material
						material = new MohrCoulomb(id, density, young, poisson, friction, cohesion, dilation, tensile, softening);	
					}

					// set up the two phases parameters
					if (ModelSetup::getTwoPhaseActive() && material!=NULL)
					{
						double density_fluid=0.0; if ((*it)["density_fluid"].is_number()) { density_fluid = ((*it)["density_fluid"]); }
						double porosity=0.0; if ((*it)["porosity"].is_number()) { porosity = ((*it)["porosity"]); }
						double bulk_fluid=0.0; if ((*it)["bulk_fluid"].is_number()) { bulk_fluid = ((*it)["bulk_fluid"]); }

						Vector3d hydraulic_conductivity(0,0,0); 
						if ((*it)["hydraulic_conductivity"].is_array()) { 

							hydraulic_conductivity.x() = ((*it)["hydraulic_conductivity"])[0];
							hydraulic_conductivity.y() = ((*it)["hydraulic_conductivity"])[1];
							hydraulic_conductivity.z() = ((*it)["hydraulic_conductivity"])[2];
						}

						material->setDensityFluid(density_fluid);
						material->setPorosity(porosity);
						material->setBulkModulusFluid(bulk_fluid);
						material->setHydraulicConductivity(hydraulic_conductivity);
					}

					if (material!=NULL){

						// set up the current material
						materials.push_back(material);
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
				
			// sphere body
			if ((*it)["type"] == "sphere") {

				// body id
				int id = 0;
				if ((*it)["id"].is_number()) {
					id = ((*it)["id"]);
				} else {
					throw(0);
				}

				// center of the sphere
				Vector3d center = Vector3d::Zero();
				if ((*it)["center"].is_array()) {
					center(0) = (*it)["center"][0];
					center(1) = (*it)["center"][1];
					center(2) = (*it)["center"][2];
				} else {
					throw(0);
				}

				// diameter of the sphere
				double diameter = 0.0;
				if ((*it)["diameter"].is_number()) {
					diameter = ((*it)["diameter"]);
				} else {
					throw(0);
				}

				// material id
				int material_id = 0;
				if ((*it)["material_id"].is_number()) {
					material_id = ((*it)["material_id"]);
				} else {
					throw(0);
				}

				// initial velocity
				Vector3d initial_velocity = Vector3d::Zero();
				if (!(*it)["initial_velocity"].is_null() && (*it)["initial_velocity"].is_array()) {
					initial_velocity(0) = (*it)["initial_velocity"][0];
					initial_velocity(1) = (*it)["initial_velocity"][1];
					initial_velocity(2) = (*it)["initial_velocity"][2];
				}

				// particles per direction (e.g., Vector3i(2, 2, 2) for 8 particles per cell)
				Vector3d particles_in_direction(2, 2, 2);  // Default to 2 particles in each direction (8 per cell)
				if ((*it)["particles_per_direction"].is_array()) {
					particles_in_direction(0) = (*it)["particles_per_direction"][0];
					particles_in_direction(1) = (*it)["particles_per_direction"][1];
					particles_in_direction(2) = (*it)["particles_per_direction"][2];
				}

				// create a new sphere
				BodySphere* iBody = new BodySphere();
				if (iBody == NULL) {
					throw(0);
				} else {
					iBody->setId(id);
					iBody->setCenter(center);
					iBody->setDiameter(diameter);
					iBody->setMaterialId(material_id);
					iBody->setInitialVelocity(initial_velocity);
					iBody->setParticlesPerDirection(particles_in_direction);
				}

				bodies.push_back(iBody);
}
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

					// initial velocity
					Vector3d initial_velocity=Vector3d::Zero(); 
					if (!(*it)["initial_velocity"].is_null() && (*it)["initial_velocity"].is_array())
					{
					 	initial_velocity(0) = (*it)["initial_velocity"][0];
						initial_velocity(1) = (*it)["initial_velocity"][1];
						initial_velocity(2) = (*it)["initial_velocity"][2];
					}

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
						iBody->setInitialVelocity(initial_velocity);
					}

					bodies.push_back(iBody);
				}

				// 2d polygon type
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

				// particle list from external file
				if ((*it)["type"] == "particles_from_file") {
					// body id
					int id = 0;
					if ((*it)["id"].is_number()) {
						id = ((*it)["id"]);
					} else {
						throw(0);
					}

					// material id
					int material_id = 0;
					if ((*it)["material_id"].is_number()) {
						material_id = ((*it)["material_id"]);
					} else {
						throw(0);
					}

					// external file name
					std::string filename;
					if ((*it)["file"].is_string()) {
						filename = (*it)["file"];
					} else {
						throw(0);
					}

					// open the file
					std::ifstream file_stream(filename);
					if (!file_stream.is_open()) {
						Warning::printMessage("Cannot open particle JSON file: " + filename);
						throw(0);
					}

					json json_particles;
					file_stream >> json_particles;

					// particle list
					std::vector<Vector3d> particles_position;
					std::vector<double> particles_volume;

					for (const auto& p : json_particles) {
						if (!p.contains("position") || !p.contains("volume")) {
							Warning::printMessage("Invalid particle format in: " + filename);
							throw(0);
						}
						Vector3d pos(p["position"][0], p["position"][1], p["position"][2]);
						double vol = p["volume"];
						particles_position.push_back(pos);
						particles_volume.push_back(vol);
					}

					// create the body
					BodyParticle* iBody = new BodyParticle();
					if (iBody == NULL) {
						throw(0);
					} else {
						iBody->setId(id);
						iBody->setMaterialId(material_id);
						bool is_two_phase = ModelSetup::getTwoPhaseActive();
						std::vector<Particle*> particle_list;
						for (size_t i = 0; i < particles_position.size(); ++i) {
							Vector3d pt1 = particles_position[i];
							double particleSize = std::pow(particles_volume[i], 1.0 / 3.0);
							particle_list.push_back(is_two_phase ?
								new ParticleMixture(pt1, NULL, Vector3d(particleSize, particleSize, particleSize)) :
								new Particle(pt1, NULL, Vector3d(particleSize, particleSize, particleSize)));
						}
						iBody->insertParticles(particle_list);
					}
					bodies.push_back(iBody);
				}

				// particle list body type
				if ((*it)["type"]=="particles")
				{
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

					// particle list
					
					// particle's material id
					std::vector<unsigned> paricles_id;
					if ((*it)["particles"]["id"].is_array()) 
					{
					 	for (size_t i = 0; i < (*it)["particles"]["id"].size(); ++i)
					 	{
					 		// particle id
					 		paricles_id.push_back((*it)["particles"]["id"].at(i));
					 	}
					}
					else
						throw(0);

					// particle position
					std::vector<Vector3d> particles_position;
					if ((*it)["particles"]["position"].is_array()) 
					{
					 	for (size_t i = 0; i < (*it)["particles"]["position"].size(); ++i)
					 	{
					 		// particle position
					 		double px = (*it)["particles"]["position"].at(i).at(0);
					 		double py = (*it)["particles"]["position"].at(i).at(1);
					 		double pz = (*it)["particles"]["position"].at(i).at(2);

					 		particles_position.push_back(Vector3d(px,py,pz));
					 	}
					}
					else
						throw(0);

					// particle volume
					std::vector<double> particles_volume;
					if ((*it)["particles"]["volume"].is_array()) 
					{
					 	for (size_t i = 0; i < (*it)["particles"]["volume"].size(); ++i)
					 	{
					 		// particle volume
					 		particles_volume.push_back((*it)["particles"]["volume"].at(i));
					 	}
					}
					else
						throw(0);

					// create the body
					BodyParticle* iBody = new BodyParticle();

					if (iBody==NULL)
					{
						throw(0);
					}
					else
					{
						iBody->setId(id);
						iBody->setMaterialId(material_id);
						unsigned n_particles = static_cast<unsigned int>((*it)["particles"]["id"].size());
						std::vector<Particle*> particle_list;
						bool is_two_phase = ModelSetup::getTwoPhaseActive();
						for (size_t i = 0; i < n_particles; ++i)
						{
							Vector3d pt1 = particles_position.at(i);
							double particleSize = std::pow(particles_volume.at(i), 1.0/3.0);
							particle_list.push_back( is_two_phase ? (new ParticleMixture(pt1,NULL,Vector3d(particleSize,particleSize,particleSize))) : (new Particle(pt1,NULL,Vector3d(particleSize,particleSize,particleSize))));
						}
						iBody->insertParticles(particle_list);
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
			fields.push_back("all");
			return fields;
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
		if (inputFile["damping"].is_null()){
			return ModelSetup::DampingType::UNDAMPED;
		}

		if (inputFile["damping"]["type"].is_null()){
			throw(0);
		}
		
		if(inputFile["damping"]["type"]=="local"){
			return ModelSetup::DampingType::LOCAL;
		}

		if(inputFile["damping"]["type"]=="kinetic"){
			return ModelSetup::DampingType::KINETIC_DYNAMIC_RELAXATION;
		}

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
	else if (resPlane=="earthquake")
	{
		restrictions.at(index)=Boundary::BoundaryType::EARTHQUAKE;
		ModelSetup::setSeismicAnalysis(true);
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

NodalPointLoadData Input::readNodalPointLoads( ) {

    Loads::NodalPointLoadData nodalPointLoads;

    if (inputFile.contains("nodal_point_load")) {

        for (const auto& item : inputFile["nodal_point_load"]) {

            if (item.size() == 2 && item[0].is_array() && item[1].is_array() && 
                item[0].size() == 3 && item[1].size() == 3) {

				Vector3d point, load;
				int id;

                for (size_t i = 0; i < 3; ++i)
				{
                    point[i] = item[0][i];
                    load[i] = item[1][i];
					id = -1; // this must be configured with the mesh
                }

				nodalPointLoads.points.push_back(point);
				nodalPointLoads.loads.push_back(load);
				nodalPointLoads.nodal_ids.push_back(id);

            } else {
                throw std::runtime_error("Invalid structure in nodal_point_load");
            }
        }
    }
    return nodalPointLoads;
}

vector<Loads::LoadDistributedBox> Input::getLoadDistributedBox() {

	try{
		vector<Loads::LoadDistributedBox> load_box_distributed;

		string key = "load_distributed_box";

		if (!inputFile[key].is_null()){
		
			json::iterator it;
			for(it = inputFile[key].begin(); it!=inputFile[key].end();it++) {

				Loads::LoadDistributedBox iload;

				Vector3d p1(0,0,0), p2(0,0,0), load(0,0,0); 
				
				if ((*it)["point_p1"].is_array()) { 
					p1.x() = ((*it)["point_p1"])[0]; 
					p1.y() = ((*it)["point_p1"])[1]; 
					p1.z() = ((*it)["point_p1"])[2];

					iload.pointP1 = p1;
				}

				if ((*it)["point_p2"].is_array()) { 
					p2.x() = ((*it)["point_p2"])[0]; 
					p2.y() = ((*it)["point_p2"])[1]; 
					p2.z() = ((*it)["point_p2"])[2]; 

					iload.pointP2 = p2;
				}

				if ((*it)["load"].is_array()) { 
					load.x() = ((*it)["load"])[0]; 
					load.y() = ((*it)["load"])[1]; 
					load.z() = ((*it)["load"])[2]; 

					iload.load = load;
				}

				load_box_distributed.push_back(iload);
			}
		}

		return load_box_distributed;
	}
	catch(...)
	{
		Warning::printMessage("Error in load box definition");
		throw;
	}
}

vector<Loads::PressureBox> Input::getPrescribedPressureBox() {

	try{
		vector<Loads::PressureBox> pressure_box_list;

		string key = "prescribed_pressure_box";

		if (!inputFile[key].is_null()){
		
			json::iterator it;
			for(it = inputFile[key].begin(); it!=inputFile[key].end();it++) {

				Loads::PressureBox ipressure_box;

				Vector3d p1(0,0,0), p2(0,0,0);

				double pressure(0); 
				
				if ((*it)["point_p1"].is_array()) { 
					p1.x() = ((*it)["point_p1"])[0]; 
					p1.y() = ((*it)["point_p1"])[1]; 
					p1.z() = ((*it)["point_p1"])[2];

					ipressure_box.pointP1 = p1;
				}

				if ((*it)["point_p2"].is_array()) { 
					p2.x() = ((*it)["point_p2"])[0]; 
					p2.y() = ((*it)["point_p2"])[1]; 
					p2.z() = ((*it)["point_p2"])[2]; 

					ipressure_box.pointP2 = p2;
				}

				if ((*it)["pressure"].is_number()) { 
					pressure = ((*it)["pressure"]); 

					ipressure_box.pressure = pressure;
				}

				pressure_box_list.push_back(ipressure_box);
			}
		}

		return pressure_box_list;
	}
	catch(...)
	{
		Warning::printMessage("Error in prescribed pressure box definition");
		throw;
	}
}

vector<Loads::PressureBox> Input::getInitialPressureBox() {

	try{
		vector<Loads::PressureBox> pressure_box_list;

		string key = "initial_pressure_box";

		if (!inputFile[key].is_null()){
		
			json::iterator it;
			for(it = inputFile[key].begin(); it!=inputFile[key].end();it++) {

				Loads::PressureBox ipressure_box;

				Vector3d p1(0,0,0), p2(0,0,0);

				double pressure(0); 
				
				if ((*it)["point_p1"].is_array()) { 
					p1.x() = ((*it)["point_p1"])[0]; 
					p1.y() = ((*it)["point_p1"])[1]; 
					p1.z() = ((*it)["point_p1"])[2];

					ipressure_box.pointP1 = p1;
				}

				if ((*it)["point_p2"].is_array()) { 
					p2.x() = ((*it)["point_p2"])[0]; 
					p2.y() = ((*it)["point_p2"])[1]; 
					p2.z() = ((*it)["point_p2"])[2]; 

					ipressure_box.pointP2 = p2;
				}

				if ((*it)["pressure"].is_number()) { 
					pressure = ((*it)["pressure"]); 

					ipressure_box.pressure = pressure;
				}

				pressure_box_list.push_back(ipressure_box);
			}
		}

		return pressure_box_list;
	}
	catch(...)
	{
		Warning::printMessage("Error in initial pressure box definition");
		throw;
	}
}

vector<Loads::PressureMaterial> Input::getInitialPressureMaterial() {

	try{
		vector<Loads::PressureMaterial> pressure_material_list;

		string key = "initial_pressure_material";

		if (!inputFile[key].is_null()){
		
			json::iterator it;
			for(it = inputFile[key].begin(); it!=inputFile[key].end();it++) {

				Loads::PressureMaterial ipressure_material;
				
				if ((*it)["material"].is_number()) { 

					ipressure_material.materialId = ((*it)["material_id"]);
				}

				if ((*it)["pressure"].is_number()) { 

					ipressure_material.pressure = ((*it)["pressure"]); 
				}

				pressure_material_list.push_back(ipressure_material);
			}
		}

		return pressure_material_list;
	}
	catch(...)
	{
		Warning::printMessage("Error in initial pressure by material definition");
		throw;
	}
}

vector<Loads::PressureBoundaryForceBox> Input::getPressureBoundaryForceBox() {

	try {
		vector<Loads::PressureBoundaryForceBox> pressure_box_list;

		string key = "pressure_force_box";

		if (!inputFile[key].is_null()) {

			json::iterator it;
			for (it = inputFile[key].begin(); it != inputFile[key].end(); it++) {

				Loads::PressureBoundaryForceBox ipressure_box;

				Vector3d p1(0, 0, 0), p2(0, 0, 0), force(0, 0, 0);

				if ((*it)["point_p1"].is_array()) {
					p1.x() = ((*it)["point_p1"])[0];
					p1.y() = ((*it)["point_p1"])[1];
					p1.z() = ((*it)["point_p1"])[2];

					ipressure_box.pointP1 = p1;
				}

				if ((*it)["point_p2"].is_array()) {
					p2.x() = ((*it)["point_p2"])[0];
					p2.y() = ((*it)["point_p2"])[1];
					p2.z() = ((*it)["point_p2"])[2];

					ipressure_box.pointP2 = p2;
				}

				if ((*it)["pressure_force"].is_array()) {
					force.x() = ((*it)["pressure_force"])[0];
					force.y() = ((*it)["pressure_force"])[1];
					force.z() = ((*it)["pressure_force"])[2];

					ipressure_box.pressureForce = force;
				}

				pressure_box_list.push_back(ipressure_box);
			}
		}

		return pressure_box_list;
	}
	catch (...)
	{
		Warning::printMessage("Error in pressure force box definition");
		throw;
	}
};

double Input::getFrictionCoefficient() {

	try
	{
		if (inputFile["terrain_contact"].is_null()){

			return 0.0;
		}

		if (inputFile["terrain_contact"]["friction"].is_null()){

			return 0.0;
		}

		if (inputFile["terrain_contact"]["friction"].is_number())
		{
			return inputFile["terrain_contact"]["friction"];
		}

		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error during reading the friction coefficient in terrain contact");
		throw;
	}
}

bool Input::getTerrainContactActive()
{
	try
	{
		if (inputFile["terrain_contact"].is_null()){

			return false;
		}

		if (inputFile["terrain_contact"]["active"].is_null()){

			return false;
		}

		if (inputFile["terrain_contact"]["active"].is_boolean())
		{
			return inputFile["terrain_contact"]["active"];
		}

		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error during reading the terrain contact active keyword");
		throw;
	}
}

std::string Input::getSTLMeshFile()
{
	try
	{
		if (inputFile["terrain_contact"]["stl_mesh"].is_null())
		{
			throw(0);
		}

		if (inputFile["terrain_contact"]["stl_mesh"].is_string())
		{
			return inputFile["terrain_contact"]["stl_mesh"];
		}

		throw(0);
	}
	catch(...)
	{
		Warning::printMessage("Error during reading the STL file name in terrain contact");
		throw;
	}
}

SeismicData Input::readSeismicData(const std::string& filename, bool hasHeader = false) {
	
	SeismicData data;
	
	std::ifstream file(filename);
	
	if (!file.is_open()) {
		std::cerr << "Error during opening seismic data: " << filename << std::endl;
		return data;
	}

	std::string line;

	// ignore headers if we have ones
	if (hasHeader && std::getline(file, line)) {
		// headers manipulations if needed 
	}

	while (std::getline(file, line)) {
	
		std::stringstream ss(line);
		std::string item;
		double t;
		Eigen::Vector3d acc(0.0, 0.0, 0.0);

		// time
		if (!std::getline(ss, item, ',')) continue;
		t = std::stod(item);

		// ax
		if (!std::getline(ss, item, ',')) continue;
		acc.x() = std::stod(item);

		// ay
		if (!std::getline(ss, item, ',')) continue;
		acc.y() = std::stod(item);

		// az
		if (!std::getline(ss, item, ',')) continue;
		acc.z() = std::stod(item);

		data.time.push_back(t);
		data.acceleration.push_back(acc);
	}

	file.close();
	
	return data;
}