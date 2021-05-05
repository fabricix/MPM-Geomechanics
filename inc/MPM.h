/*
 * MPM.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MPM_H_
#define MPM_H_

#include "Mesh.h"
#include "Body.h"
#include "Model.h"
#include "Output.h"
#include "Input.h"
#include "Interpolation.h"
#include "States.h"

/// \class MPM
/// \brief Components and algorithms of the MPM
/// This class contain the basic objects of a MPM model,
/// and the operations between them.
class MPM {

public:

	MPM(); //!< default constructor
	virtual ~MPM(); //!< default destructor
	
public:
	
	Mesh mesh; //!< grid mesh
	vector<Body> bodies; //!< bodies discretized by material points
	Interpolation interpolation; //!< interpolation operations
	States states; //!< operations for load and save model's states
	Input input; //!< input operations
	Output output; //!< output operations
};

#endif /* MPM_H_ */
