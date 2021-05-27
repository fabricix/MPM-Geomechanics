/*
 * Output.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "Mesh.h"
#include "Particle.h"
#include "Body.h"

/// \namespace Output
/// \brief Operations to write the results and the mesh for its visualization.
namespace Output {

	/// \enum CellType
	/// Describes the way that the cells are written in the mesh file
	enum CellType{POINTS,CELLS};

	/// \brief Write the grid mesh into a vtu file
	/// \param[in] mesh Mesh reference
	/// \param[in] cell_type_representation Cell type representation Output::CellType
	void writeGrid(Mesh&,CellType=CellType::CELLS);
	
	/// \brief Write the particles of the model into a vtu file
	/// \param[in] particles_list List containing pointers to particles
	/// \param[in] time Time
	void writeParticles(vector<Particle*>&, double=0.0);
	
	/// \brief Write the particles in a body
	/// \param[in] body Body reference
	/// \param[in] time Time
	void writeBody(Body&, double=0.0);
	
	/// \brief Write the time series of the results
	///
	void writeResultsSeries();
	
	/// \brief Write the particles forming bodies
	/// \param[in] body_list_reference Body list
	/// \param[in] time Time
	void writeBodies(vector<Body*>& bodies, double=0.0);
};

#endif /* OUTPUT_H_ */