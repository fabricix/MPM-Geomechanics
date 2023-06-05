/*
 * MohrCoulombTest.cpp
 *
 *  Created on: 7 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;

#include "Eigen/Core"
using Eigen::Vector3d;
using Eigen::Vector3i;

#include "Materials/MohrCoulomb.h"
#include "Particle/Particle.h"

int main(int argc, char **argv)
{
	// model parameters
	double young = 450;
	double poisson = 0.125;
	double friction = 10.0;
	double cohesion = 1.0;
	double dilation = 10.0;
	double density = 1.0;
	double tensile = 0.0;

	// time and unloading
	double dt = 1;
	double time = 1800;
	double unloadingTime = 1000;
	bool unloadingFlag = true;
	double steps = int(time/dt);

	// deformation velocity
	double velocity = -1.0e-5;

	// create a material
	MohrCoulomb* material = new MohrCoulomb(1,density,young,poisson,friction,cohesion,dilation,tensile);

	// particle position
	Vector3d position(0,0,0);

	// particle size
	Vector3d size(1,1,1);

	// create a particle
	Particle particle = Particle(position,material,size);

	// result vectors
	vector<double> resultStress, resultStrain;

	// perform strain increment and compute the corresponding stresses
	for(int istep=0; istep<steps; istep++)
	{
		// check unloading condition
		if(unloadingFlag && istep*dt>unloadingTime)
		{
			velocity *= -1;
			unloadingFlag = false;
		}

		// compute the current deformation increment
		Matrix3d dstrain = Matrix3d::Zero();
		dstrain(0,0)=velocity/size(0)*dt;

		// set strain increment in particle
		particle.setStrainIncrement(dstrain);

		// compute the current stress
		particle.updateStress();

		// store the results
		resultStress.push_back(particle.getStress()(0,0));
		resultStrain.push_back(particle.getStrain()(0,0));
	}

	// write the results in a csv file
	ofstream outputfile;
	outputfile.open("MohrCoulombVerificationMPM.csv");
	outputfile<<"stress,strain\n";

	for(size_t i=0; i<resultStrain.size();i++){

		outputfile<<resultStress.at(i)<<","<<resultStrain.at(i)<<"\n";
	}

	outputfile.close();
    return 0;
}
