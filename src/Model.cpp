/*
 * Model.cpp
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Model.h"

Model::Model() {
	// TODO Auto-generated constructor stub

	contact=false;
	gravity=false;
	axisymetric=false;
	coupled=false;
	jaumann=false;

	istep=0;
	nThreads=0;

	dt=0.0;
	time=0.0;
	dtFraction=0.0;
	kineticEnergy=0.0;

	inputFile="";

    #if defined (_WIN64) || defined(_WIN32)
    operationalSystem=ModelSetup::OperationalSystem::WINDOWS;
    #elif defined (linux) || defined(__linux__)
    operationalSystem=ModelSetup::OperationalSystem::LINUX;
    #endif

	stressScheme=ModelSetup::StressUpdateScheme::USL;
	damping=ModelSetup::DampingType::NONE;
}

Model::~Model() {
	// TODO Auto-generated destructor stub
}

bool Model::isWindowsSystem(){
	return operationalSystem==ModelSetup::OperationalSystem::WINDOWS?true:false;
}

bool Model::isLinuxSystem(){
	return operationalSystem==ModelSetup::OperationalSystem::LINUX?true:false;
}
