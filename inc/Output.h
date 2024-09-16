/*
 * Output.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "Mesh/Mesh.h"
#include "Particle/Particle.h"
#include "Body/Body.h"

/// \namespace Output
/// \brief Operations to write the results and the mesh for its visualization.
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
	void writeGrid(Mesh* mesh, CellType cell_type_representation=CellType::CELLS);
	
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
	
	/// \brief Write the particles forming bodies
	/// \param[in] bodies Body list
	/// \param[in] time Time
	void writeBodies(vector<Body*>* bodies, double time=0.0);

	/// \brief Configures the fields to be written
	/// \param[in] fields List of fields
	void configureResultFiels(vector<string> fields);
	
	/// \brief Verify is the field must be written
	/// \param[in] field Field to verify
	/// \return True if the field must be written
	bool isFieldRequired (string field);

	/// \brief Write results if the step is the step result
	/// 
	void writeResultInStep(int loopCounter, int resultSteps, vector<Body*>* bodies, double iTime);

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
