// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef MPM_H_
#define MPM_H_

#include "Mesh/Mesh.h"
#include "Body/Body.h"
#include "Model.h"
#include "Output.h"
#include "Input.h"
#include "States.h"
#include "Solver/Solver.h"
#include "TerrainContact.h"

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
	
private:
	
	Mesh mesh; //!< background grid mesh

	TerrainContact* terrainContact; //!< terrain contact object

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
	
	/// \brief Configure Terrain contact
	void setupTerrainContact();
	
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

	/// \brief Get seismic analysis active
	bool getSeismicAnalysis();

	/// \brief Set seismic analysis active
	void setSeismicAnalysis(bool);

	void setupParticlesPerThread();
};

#endif /* MPM_H_ */