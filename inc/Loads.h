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

	void setGravity(vector<Body*>&); //!< configures the gravity load in particles

};

#endif /* LOADS_H_ */
