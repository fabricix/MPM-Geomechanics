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

	void writeGrid(Mesh&,CellType=CellType::CELLS); //!< write the grid mesh into a vtu file.
	void writeParticles(vector<Particle>&, double=0.0); //!< write the particles of the model into a vtu file.
	void writeBody(Body&, double=0.0); //!< write the particles in a body.
	void writeResultsSeries(); //!< write the time series of the results
};

#endif /* OUTPUT_H_ */
