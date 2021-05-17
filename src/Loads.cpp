/*
 * Loads.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Loads.h"
#include "Model.h"

namespace Loads{

	void setGravity(vector<Body>& bodies){

		if (!ModelSetup::getGravityActive()){

			return;
		}

		for (size_t i = 0; i < bodies.size(); ++i)
		{
			for (size_t j = 0; j < bodies.at(i).getParticles().size(); ++j)
			{
				bodies.at(i).getParticles().at(j).addExternalForce(
						bodies.at(i).getParticles().at(j).getMass()*ModelSetup::getGravity());
			}
		}
	}
}

