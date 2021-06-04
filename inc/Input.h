/*
 * Input.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <string>
using std::string;

#include <map>
using std::map;

#include "Json/json.hpp"
using json = nlohmann::json;

#include "Eigen/Core"
using Eigen::Vector3i;
using Eigen::Vector3d;

#include "Solver.h"
#include "Material.h"
#include "Model.h"

/// \namespace Input
/// \brief Operations to read the input file.
namespace Input {

	/// \enum KeyWords
	/// \brief Defines all the keywords used in the input file.
	enum KeyWords
	{
		alpha, //!< alpha damping parameter
		body, //!< used to define a body
		boundaryConditons, //!< used to define the boundary conditions
		cellDimension, //!< used to define the cell dimension in each direction
		cohesion, //!< cohesion parameter in elasto-plastic model
		cuboid, //!< used to define a cuboid body
		damping, //!< used to define the damping in the model
		density, //!< mass density
		displacement, //!< used to write the displacement field in the output file
		timeStepFraction, //!< used to define the fraction of the critical time step
		elastic, //!< used to define an elastic body
		fields, //!< used to inform the field to be written in the output file
		fixed, //!< used to define a fixed boundary condition
		friction, //!< friction angle parameter in an elasto-plastic model
		gravity, //!< body force intensity
		GIMP, //!< generalized interpolation material point
		id, //!< used to define an identification
		linear,//!< linear shape function
		localNoViscous, //!< used to define a local no viscous damping
		mass, //!< used to define the mass of a particle
		materialId, //!< used to define a material identification
		material, //!< used to define materials
		mesh, //!< used to define a mesh
		nCells, //!< number of cells in each direction
		nThreads, //!< number of threads in the current simulation
		number, //!< number of results to be written
		origin, //!< used to define the origin of coordinates
		paneX0, //!< used to define the boundary conditions in the plane X0
		paneXn, //!< used to define the boundary conditions in the plane Xn
		paneY0, //!< used to define the boundary conditions in the plane Y0
		paneYn, //!< used to define the boundary conditions in the plane Yn
		paneZ0, //!< used to define the boundary conditions in the plane Z0
		paneZn, //!< used to define the boundary conditions in the plane Zn
		particle, //!< used to define the total particles
		plastic, //!< defines an elasto-plastic material
		pointP1, //!< defines the a lower left point in a cuboid
		pointP2, //!< defines the a higher right point in a cuboid
		poisson, //!< defines the Poisson's ratio in a material
		position, //!< define position using coordinates
		results, //!< used to define the type and the number of results
		shapeFunction, //!< nodal shape functions
		sliding, //!< used to defines a sliding boundary condition
		stress, //!< used to inform the field stress in to be written
		stressSchemeUpdate, //!< defines the type of stress update
		structured, //!< used to defines the mesh type
		time, //!< simulation time
		timeStep, //!< time step
		type, //!< used to define a type of mesh
		USL, //!< used to inform the Update Stress Last scheme
		young //!< Young's modulus of an elastic material 
	};

	/// \brief Initialize the keywords
	///
	void initKeyWords();
	
	/// \brief Read the input file
	///
	void readInputFile();
	
	/// \brief Read the input file
	/// \param[in] file_name File name
	void readInputFile(string file_name);

	/// \brief Return the map with the keywords
	/// \return A map containing an string for each KeyWord
	const map<Input::KeyWords,string>& getKeyWords();
	
	/// \brief Return the data file structure
	/// \return Json file structure containing all read data
	const json& getJson();

	/// \brief Return the file name
	/// \return File name
	string getFileName();
	
	/// \brief Configure the filename
	/// \param[in] file_name File name
	void setFileName(string file_name);
	
	/// \brief Return the simulation time
	/// \return Simulation time
	double getSimulationTime();
	
	/// \brief Return the time step
	/// \return Time step
	double getTimeStep();
	
	/// \brief Return the solver to be used in the model.
	///
	/// After read the input file this class creates an Solver instance and returns its pointer.
	/// \return Solver
	Solver* getSolver();
	
	/// \brief Return the interpolation functions type
	/// \return ModelSetup::InterpolationFunctionType
	ModelSetup::InterpolationFunctionType getInterpolationFunction();
	
	/// \brief Return the number of cells in each direction
	/// \return Number of cells in each direction
	Vector3i getCellsNum();
	
	/// \brief Return the cell dimension
	/// \return Cell dimension in each direction 
	Vector3d getCellDimension();
	
	/// \brief Return the origin of coordinates
	/// \return Coordinates of the origin of the mesh
	Vector3d getOrigin();
	
	/// \brief Return the material list
	/// \return A list containing Material pointers
	vector<Material*> getMaterialList();
	
	/// \brief Return the body list
	/// \return A list containing Body pointers
	vector<Body*> getBodyList();
	
	/// \brief Return the gravity force
	/// \return Gravity force
	Vector3d getGravity();
	
	/// \brief Return the number of results
	/// \return Number of results to be written
	int getResultNum();

	/// \brief Verifies the input data 
	/// \param[in] jsonObject Structure to be verified
	/// \param[in] keyword Keyword to be verified
	/// \param[in] dataType String of the data type to be verified:
	/// this can be "string", "number", "array" or "boolean"
	/// \return status True is the file contains the keyword with the specified dataType 
	bool verifyData(json jsonObject, Input::KeyWords keyword, string dataType );

	/// \brief Verifies the input data 
	/// \param[in] jsonObject Structure to be verified
	/// \param[in] keyword Keyword to be verified
	/// \return status True is the file contains the keyword with the specified dataType
	bool verifyData(json jsonObject, Input::KeyWords keyword );
};

#endif /* INPUT_H_ */
