/*
 * testParticleContribution.cpp
 *
 *  Created on: 24 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;

#include <string>
using std::to_string;

#include "MPM.h"
#include "Particle.h"

int main(int argc, char **argv)
{
	// create the mesh
	Mesh mesh;
	mesh.setNumCells(10,10,10);
	mesh.setCellDimension(1,1,1);
	mesh.setNumGhosts(2);
	mesh.createGrid();

	// create particles
	std::vector<Particle> particles;
	particles.push_back(Particle(Vector3d(0.0,0.0,0.0)));
	particles.push_back(Particle(Vector3d(2.1,5.2,7.4)));
	particles.push_back(Particle(Vector3d(1.4,3.2,8.4)));

	// configures particle size
	for (size_t i=0; i<particles.size(); i++){
		particles.at(i).setSize(Vector3d(1.0,1.0,1.0));
	}

	// update contributions and write results
	for (size_t i=0; i<particles.size(); i++){

		// get particle
		Particle iparticle = particles.at(i);

		// update particle contribution
		iparticle.updateContributionNodes(mesh);

		// create an output file by particle
		ofstream outputfile;
		outputfile.open ("particle"+to_string(i)+".csv");
		outputfile<<"node,w,gx,gy,gz\n";

		for (size_t j=0; j<iparticle.getContributionNodes()->size(); j++){

			int nodeId = iparticle.getContributionNodes()->at(j).getNodeId();
			double weight = iparticle.getContributionNodes()->at(j).getWeight();
			double gradiX = iparticle.getContributionNodes()->at(j).getGradients().x();
			double gradiY = iparticle.getContributionNodes()->at(j).getGradients().y();
			double gradiZ = iparticle.getContributionNodes()->at(j).getGradients().z();
			outputfile<<nodeId<<","<<weight<<","<<gradiX<<","<<gradiY<<","<<gradiZ<<"\n";
		}
		outputfile.close();
	}
	return 0;
}


