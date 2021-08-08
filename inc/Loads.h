/*
 * Loads.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef LOADS_H_
#define LOADS_H_

#include "Body/Body.h"

#include <vector>
using std::vector;

/// \namespace Loads
/// \brief Operations to manage loads in the MPM model.
namespace Loads {

    /// \struct LoadDistributedBox
    /// \brief Structure to define a distributed load in 
    /// particles inside the cuboid defined by point p1 and p2
    struct LoadDistributedBox
    {
        Vector3d pointP1;
        Vector3d pointP2;
        Vector3d load;
    };

	/// \brief Configures the gravity load in particles
	/// \param[in] bodies A list containing Body pointers 
	void setGravity(vector<Body*>& bodies);
    
    /// \brief Configure distributed load in particles
    /// \param[in] bodies A list containing Body pointers 
    /// \param[in] loads A list containing loads to be applied
    void setLoadDistributedBox(vector<Body*>& bodies, vector<Loads::LoadDistributedBox> loads);

};

#endif /* LOADS_H_ */
