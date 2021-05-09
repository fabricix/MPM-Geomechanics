/*
 * testParticleContribution.cpp
 *
 *  Created on: 24 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Elastic.h"

int main(int argc, char **argv)
{

	// create a basic class material
	Material material(10,1.0);

	// create an elastic material
	Elastic elastic(10,1.0,1.0,0.3);

	// create a material pointer
	Material* materialPntr=0;
		
	// create an elastic pointer
	Elastic* elasticPntr=0;

	// updating the stress in derivated class from derivated pointer
	elasticPntr=&elastic;
	elasticPntr->updateStress();


	// updating the stress in derivated class from base class pointer
	materialPntr=&elastic;
	materialPntr->updateStress();

	// updating the stress after assaign the derivate class object to a base object
	material=elastic;
	material.updateStress();

	return 0;
}


