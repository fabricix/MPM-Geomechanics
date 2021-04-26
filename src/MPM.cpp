/*
 * MPM.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "MPM.h"

MPM::MPM() {


}

MPM::~MPM() {
	// TODO Auto-generated destructor stub
}

void MPM::printMesh()
{
	output.writeGrid(mesh);
}

void MPM::updateParticleContribution()
{
	for (size_t i = 0; i < particles.size(); ++i)
	{
		particles.at(i).updateContributionNodes(mesh);
	}
}
