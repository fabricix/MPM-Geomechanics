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

#include "Json/json.hpp"
using json = nlohmann::json;

#include "Eigen/Core"
using Eigen::Vector3i;
using Eigen::Vector3d;

#include "Solver/Solver.h"
#include "Materials/Material.h"
#include "Model.h"
#include "Boundary.h"
#include "Loads.h"

/// \namespace Input
/// \brief Operations to read the input file.
///
/// ## Definition
///
/// The keywords are the way to setup the numerical model. 
/// Have keyword for each characteristic implemented in the program.
/// Some of they are associated with numbers, for example `"time":0.2`, others
/// that are associated with strings, for example `"type":"elastic"`, or to 
/// arrays, for example `"gravity":[0.0,0.0,-9.81]`, and with others definitions, like `"mesh":{
/// "cells_dimension":[1,1,1], ...` 
/// Next is presented the complete list of keyword for using in the input file,
/// organized according the characteristic to be configured. 

/// ## Bodies
///	| Keyword | Description | Data type |
/// | --------| ----------- | ----------|
/// | body | define bodies | -- |
/// | type | define a type of body | string |
/// | id | define a body identification | integer |
/// | material_id | used to setup a defined material to the body | integer |

/// ### Cuboid
///	| Keyword | Description | Data type |
/// | --------| ----------- | ----------|
/// | cuboid | cuboid body type | body type |
/// | point_p1 | define the a lower left point in a cuboid | array |
/// | point_p2 | define the a higher right point in a cuboid | array |

/// #### Cuboid definition example
///
/// ```
/// "body":
/// {
/// 	"cuboid":
/// 	{
/// 		"type":"cuboid",
/// 		"id":1,
/// 		"point_p1":[2,2,2],
/// 		"point_p2":[7,7,7],
/// 		"material_id":1
/// 	}
/// }
/// ```

/// ### Polygon extrusion
///	| Keyword | Description | Data type |
/// | --------| ----------- | ----------|
/// | polygon_2d | body created by a 2D polygon | body type |
/// | extrude_direction | direction of the extrusion during the polygon body type | string |
/// | extrude_displacement| magnitude of the displacement in the extrude direction | double |
/// | discretization_length | cell dimension of the auxiliary mesh during the creation of the polygon body type  | double |
/// | points | 3D coordinate points | array |

/// #### Polygon extrusion definition example
///
///  ```
/// "body":
/// {
/// 	"soil":
/// 	{
/// 		"type":"polygon_2d",
/// 		"extrude_direction":"y",
/// 		"extrude_displacement":2,
/// 		"discretization_length":0.5,
/// 		"id":1,
/// 		"points":
/// 		[
/// 			[0,     0,      0],
/// 			[2.001, 0,      0],
/// 			[0,     0,  2.001]
/// 		],
/// 		"material_id":1
/// 	}
/// }
/// ```

/// ## Loads
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | gravity | define the body force intensity | array |
/// | load_box | distribute a load in particles inside the box | -- |

/// ### Load definition example
///	
/// #### Gravity load Example
///
/// ```
/// "gravity":[0.0,0.0,-9.81],
/// ```
/// 
/// #### Distributed load in box example
///
/// ```
///	"load_distributed_box":
///	{
///		"load_1":
///		{
///			"point_p1":[0,0,6],
///			"point_p2":[5,1,10],
///			"load":[0,0,-10]
///		}
///	}
/// ```

/// ## Damping
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | damping | define the damping type | -- |
/// | local | define the local damping value | double |

/// ### Damping definition example
///
/// ```
/// "damping":
/// {
/// 	"type":"local",
/// 	"value":0.0
/// }
/// ```

/// ## Parallelization
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | n_threads | define the number of threads in  the simulation | unsigned |

/// ### Paralelization definition example
///
/// ```
/// "n_threads":1,
/// ```

/// ## Simulation time
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | time | define the simulation time | double |
/// | time_step | define the time step | double |
///

/// ## Simulation time definition example
///
/// ```
/// "time":2,
/// ```

/// ## Two phase simulations
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | n_phases | define number of phases in the simulations (default is 1) | int |

/// ### Two phase simulation definition example
///
/// ```
/// "n_phases":2,
/// ```

/// ## Stress update scheme
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | stress_scheme_update | used to define the type of stress update scheme| string |
/// | USL | Update Stress Last scheme type | -- |

/// ### Stress update scheme definition example
/// 
/// ```
/// "stress_scheme_update":"USL",
/// ```

/// 
/// ## Shape Functions
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | shape_function | define the type of nodal interpolation function | string |
/// | GIMP | generalized interpolation material point shape function type | -- |
/// | linear | linear shape function type | -- | 

/// ### Shape functions definition example
///
/// ```
///  "shape_function":"GIMP",
/// ```

/// ## Mesh
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | mesh | used to define a mesh | -- | 
/// | cells_number | define the number of cells in each direction | array |
/// | cells_dimension | used to define the cell dimension in each direction | array |
/// | origin | used to define the origin of coordinates | array |
/// | boundary_conditions | used to set the mesh boundary conditions | -- |
/// | boundary_conditions_fluid | used to set the mesh boundary conditions of fluid | -- |
/// | sliding | boundary condition that can move perpendicular to the normal of the boundary plane | string |
/// | fixed | boundary condition that can not move in any direction | string |
/// | free | boundary condition that can move freely in all directions | string |
/// | plane_X0 | Plane which normal points to the negative part of the axis X | -- | 
/// | plane_Y0 | Plane which normal points to the negative part of the axis Y | -- | 
/// | plane_Z0 | Plane which normal points to the negative part of the axis Z | -- | 
/// | plane_Xn | Plane which normal points to the positive part of the axis X | -- | 
/// | plane_Yn | Plane which normal points to the positive part of the axis Y | -- | 
/// | plane_Zn | Plane which normal points to the positive part of the axis Z | -- | 

/// ### Mesh definition example
///
/// ```
/// "mesh":
///	{
/// 	"cells_dimension":[1,1,1],
/// 	"cells_number":[10,10,10],
/// 	"origin":[0,0,0],
///
///		"boundary_conditions":
/// 	{
/// 		"plane_X0":"fixed",
/// 		"plane_Y0":"fixed",
/// 		"plane_Z0":"fixed",
/// 		"plane_Xn":"sliding",
/// 		"plane_Yn":"sliding",
/// 		"plane_Zn":"free"
/// 	}
/// },
/// ```

/// ## Materials
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | material | used to define materials | -- |
/// | id | material identification | integer |
/// | density | mass density | double |
/// | density_fluid | mass density of fluid in mixture| double |
/// | porosity | porosity of mixture| double |
/// | bulk_fluid | volumetric modulus of fluid in mixture | double |
/// | hydraulic_conductivity | principal values of the hydraulic conductivity of fluid in mixture | array (1x3) |
/// | type | used to specify the material constitutive model | string |

/// ## Linear Elastic
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | elastic | elastic material type | -- |
/// | poisson | Poisson's ratio of the material | double |
/// | young | Young's modulus of the material | double |

/// ## Mohr-Coulomb
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | mohr-coulomb | Mohr-Coulomb elasto plastic material type | -- |
/// | friction | Angle of internal friction in degree | double |
/// | cohesion | Material Cohesion | double |
/// | dilation | Material dilation | double |
/// | tensile  | Tensile cut-off   | double |

/// ### Material definition example
///
/// ```
/// "material":
/// {
/// 	"plastic":
/// 	{
/// 		"type":"mohr-coulomb",
/// 		"id":1,
/// 		"young":50e4,
/// 		"density":2000,
/// 		"poisson":0.2,
/// 		"friction":30.0,
/// 		"cohesion":1500.0
/// 	}
/// },
/// ```

/// ## Results
///	| Keyword | Description | Data type |
/// | --------| ----------- | --------- |
/// | results | used to define the type and the number of results | -- |
/// | print | number of results to be written | integer |
/// | fields | used to define the fields to be written | string array |
/// | id | particle identification field | -- |
/// | displacement | particle displacement field  | -- |
/// | material | particle material field | -- |
/// | pressure | fluid pressure | -- |
/// | external_force | total external force in particle | -- |

/// ### Results definition example
///
/// ```
/// "results":
///	{
/// 	"print":50,
/// 	"fields":["id","displacement","material"]
/// },
/// ```

/// ## Complete input file definition example
///
/// ```
/// {
/// 	"stress_scheme_update":"USL",
///
/// 	"shape_function":"GIMP",
///
///		"n_threads":4,
///
/// 	"time":2,	
///
/// 	"time_step":0.001,
///
/// 	"gravity":[0.0,0.0,-9.81],
///
/// 	"results":
///		{
/// 		"print":50,
/// 		"fields":["id","displacement","material"]
/// 	},
///
/// 	"mesh":
///		{
/// 		"cells_dimension":[1,1,1],
/// 		"cells_number":[10,10,10],
/// 		"origin":[0,0,0],
///
///			"boundary_conditions":
/// 		{
/// 			"plane_X0":"fixed",
/// 			"plane_Y0":"fixed",
/// 			"plane_Z0":"fixed",
/// 			"plane_Xn":"sliding",
/// 			"plane_Yn":"sliding",
/// 			"plane_Zn":"free"
/// 		}
/// 	},
///
/// 	"material":
/// 	{
/// 		"elastic":
/// 		{
/// 			"type":"elastic",
/// 			"id":1,
/// 			"young":50e4,
/// 			"density":2000,
/// 			"poisson":0.2
/// 		}
/// 	},
///
/// 	"body":
/// 	{
/// 		"cuboid":
/// 		{
/// 			"type":"cuboid",
/// 			"id":1,
/// 			"point_p1":[2,2,2],
/// 			"point_p2":[7,7,7],
/// 			"material_id":1
/// 		}
/// 	}
/// }
/// ```

namespace Input {
	
	/// \brief Read the input file
	/// \param[in] file_name File name
	void readInputFile(string file_name);
	
	/// \brief Return the data file structure
	/// \return Json file structure containing all read data
	const json& getJson();

	/// \brief Return the file name
	/// \return File name
	string getFileName();
	
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

	/// \brief Return the fields to be written
	///
	vector<string> getResultFields();

	/// \brief Read the damping definition in the input file
	/// \return ModelSetup::DampingType The damping type
	ModelSetup::DampingType getDampingType();

	/// \brief Read the damping value
	/// \return value The damping value
	double getDampingValue();

	/// \brief Return the mesh boundary conditions
	/// \return boundary_restriction_vector A vector containing 
	/// the restrictions of each plane: X0, Y0, Z0, Xn, Yn, and Zn.
	vector<Boundary::BoundaryType> getMeshBoundaryConditions();
	
	/// \brief Return the mesh boundary conditions of fluid
	/// \return boundary_restriction_vector A vector containing 
	/// the restrictions of each plane: X0, Y0, Z0, Xn, Yn, and Zn.
	vector<Boundary::BoundaryType> getMeshBoundaryConditionsFluid();

	/// \brief Return number of threads defined in the input file
	/// \return n_threads Number of threads
	unsigned getNumThreads();

	/// \brief Return the number of phases in the simulation
	/// \return n_phases Number of phases in the simulation
	unsigned getNumberPhases();
	
	/// \brief Return loads distributed in particles inside a box
	/// \return loads_distributed_box  Loads distributed in particles inside a box
	vector<Loads::LoadDistributedBox> getLoadDistributedBox();
};

#endif /* INPUT_H_ */