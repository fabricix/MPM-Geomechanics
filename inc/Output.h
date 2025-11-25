// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "Mesh/Mesh.h"
#include "Particle/Particle.h"
#include "Body/Body.h"
#include "TerrainContact.h"

/// \namespace Output
/// \brief Operations to write the results and the mesh for its visualization.
///
/// The results of a numerical simulation are the variables stored in particles in time. These files are written in two folders inside the working directory: `particles` and `grid`. In `particles` folder temporal results are written in `vtu` files and additional file `particleTimeSerie.pvd` is created at the end of the simulation containing information of `vtu` files and time. 
///
namespace Output {

	/// \enum CellType
	/// Describes the way that the cells are written in the mesh file
	enum CellType{ POINTS, CELLS };

	/// \brief Initialize the keywords of the fields
	///
	void initFieldsKeyWords();

	/// \brief Write the grid mesh into a vtu file
	/// \param[in] mesh Mesh reference
	/// \param[in] cell_type_representation Cell type representation Output::CellType
	/// \param[in] time Time
	void writeGrid(Mesh* mesh, CellType cell_type_representation=CellType::CELLS, double time=0.0);
	
	/// \brief Write the particles of the model into a vtu file
	/// \param[in] particles List containing pointers to particles
	/// \param[in] time Time
	void writeParticles(vector<Particle*>* particles, double time=0.0);
	
	/// \brief Write the particles in a body
	/// \param[in] body Body reference
	/// \param[in] time Time
	void writeBody(Body* body, double time=0.0);
	
	/// \brief Write the time series of the results
	///
	void writeResultsSeries();
	
	/// \brief Write the STL contact results
	void writeSTLContact(TerrainContact* tc, double time);

	/// \brief Write the particles forming bodies
	/// \param[in] bodies Body list
	/// \param[in] time Time
	void writeBodies(vector<Body*>* bodies, double time=0.0);

	/// \brief Configures the fields to be written
	/// \param[in] fields List of fields
	void configureResultFields(vector<string> fields);

	/// \brief Configures the grid fields to be written
	/// \param[in] fields List of fields
	void configureGridResultFields(vector<string> fields);
	
	/// \brief Configures the stlContact fields to be written
	/// \param[in] fields List of fields
	void configureSTLContactFields(vector<string> fields);

	/// \brief Verify is the grid field must be written
	/// \param[in] field Grid field to verify
	/// \return True if the grid field must be written
	bool isFieldRequired(string field);

	/// \brief Verify is the field must be written
	/// \param[in] field Field to verify
	/// \return True if the field must be written
	bool isGridFieldRequired(string field);

	/// \brief Write results if the step is the step result
	/// \param[in] resultSteps Result steps
	/// \param[in] particles List of particles
	/// \param[in] iTime Current time
	void writeResultInStep(int resultSteps, vector<Particle*>* particles, double iTime);

	/// \brief Write grid results if the step is the step result
	/// 
	void writeGridInStep(int resultSteps, Mesh* mesh, double iTime);

	/// \brief Write the STL contact results if the step is the step result
	/// 
	void writeSTLContactInStep(int resultSteps, TerrainContact* tc, double iTime);

	/// \brief Write the initial state of the simulation
	/// 
	void writeInitialState(vector<Body*>* bodies, double iTime, Mesh* mesh, TerrainContact* tc);

	/// \brief Print initial program screen
	///
	void welcomeScreen();

	/// \brief Print final program screen
	///
	void farewellScreen();

	/// \brief Print elapsed time
	///
	void printElapsedTime();
}

#endif /* OUTPUT_H_ */
