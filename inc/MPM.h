
 /* MPM.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

/// \mainpage MPM-Geomechanics
///
/// \section program_intro Introduction
///
/// The MPM-Geomechanics is an implementation of the Material Point Method to simulate large strain geomechanical problems.
///
/// The main objectives of this program are:
///
/// - Provide an open source platform suitable for the study and application of MPM in various geomechanical problems.
///
/// - provide an adequate computational tool to simulate geomechanical problems involving large deformations.
///
/// \section program_features Program Features
/// The main features of the program are:
///
/// - Three-dimensional formulation
/// - Dynamic formulation
/// - Shared memory parallelization
/// - Elastic constitutive models
/// - Elasto-plastic constitutive models
/// - Softening/Hardening by deformation
/// - Coupled hydromechanical formulation (in development)
///
/// \section program_compilation Compilation
///
/// The `/make` directory contains the makefile. To compile the program run make in the `/make` folder:
///
/// `/make$ make`
///
/// \section program_documentation Documentation
///
/// The program documentation is generated using Doxygen. The DoxyFile file is located in the `/doxygen` directory. To generate the documentation, run:
///
/// `/doxygen$ doxygen`
///
/// The HTML documentation is located in the `/doxygen/html` directory. To generate the PDF file, run make inside the `doxy/latex` directory.
///
/// `/doxygen/latex$ make`
///
/// \section program_execution Execution
/// In order the run the program call it with the name of the input file, for example:
///
/// `/make$ ./MPM-Geomechanics example.json`
///

#ifndef MPM_H_
#define MPM_H_

#include "Mesh/Mesh.h"
#include "Body/Body.h"
#include "Model.h"
#include "Output.h"
#include "Input.h"
#include "States.h"
#include "Solver/Solver.h"

/// \class MPM
/// \brief Components and algorithms of the MPM
/// This class contain the basic objects of a MPM model,
/// and the operations between them.
class MPM {

public:

	/// \brief Default constructor
	///
	MPM(); 
	
	/// \brief Default destructor
	///
	virtual ~MPM();

	/// \brief Read the input file of the model
	/// \param[in] argument_counter Argument counter
	/// \param[in] argument_vector Argument vector
	void readInputFile(int argument_counter, char ** argument_vector);
	
	/// \brief Create the numerical MPM model
	///
	void createModel();
	
	/// \brief Solve the problem in time
	///
	void solve();
	
	/// \brief Finish all program structure
	///
	void end();

	/// \brief Load model state
	///
	void loadState();

	/// \brief Save model state
	///
	void saveState();

	/// @brief Configure silent particles
	void setupSilentParticles();
	
private:
	
	Mesh mesh; //!< grid mesh

	vector<Body*> bodies; //!< bodies discretized by material points

	vector<Particle*> particles; //!< material points list

	vector<Material*> materials; //!< bodies discretized by material points
	
	Solver* solver; //!< operation for solve the equations in time

	/// \brief Configure the simulation time
	///
	void setSimulationTime();
	
	/// \brief Configure the solver of the model
	///
	void setSolver();
	
	/// \brief Configure the mesh
	///
	void setupMesh();
	
	/// \brief Configures the material list
	///
	void setupMaterialList();
	
	/// \brief Configure the body list
	///
	void setupBodyList();
	
	/// \brief Create bodies
	///
	void createBodies();
	
	/// \brief Configure the time step 
	///
	void setTimeStep();

	/// \brief Configure the interpolation functions
	///
	void setInterpolationFunctions();
	
	/// \brief Configure particles
	///
	void setupParticles();
	
	/// \brief Configure loads in the model
	///
	void setupLoads();
	
	/// \brief Configure results to be written
	///
	void setupResults();
	
	/// \brief Configure damping forces
	/// 
	void setupDamping();

	/// \brief Configure number of phases in the simulation
	///
	void setNumberPhasesInSimulation();


	/// \brief Configure number of threads
	///
	void setThreads();

	bool getSeismicAnalysis();
	void setSeismicAnalysis(bool);
};

#endif /* MPM_H_ */