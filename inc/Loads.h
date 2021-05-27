/*
 * Loads.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef LOADS_H_
#define LOADS_H_

#include <Body.h>

#include <vector>
using std::vector;

/// \namespace Loads
/// \brief Operations to manage loads in the MPM model.
namespace Loads {

	/// \brief Configures the gravity load in particles
	/// \param[in] body_list A list containing Body pointers 
	void setGravity(vector<Body*>&);

};

#endif /* LOADS_H_ */
