/*
 * Model.cpp
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Model.h"

namespace ModelSetup {

	bool contact=false;				//!< is contact active
	bool gravity=false;				//!< is gravity active
	bool axisymetric=false;			//!< is axisymetric model
	bool coupled=false;				//!< is coupled analysis
	bool jaumann=false;				//!< is Jaumann rate active

	int istep=0;					//!< current time step
	int nThreads=1;					//!< number of threads in current job
	int contributionNodes=27;		//!< nodes that the particles contributed

	double dt=0.0;					//!< time step
	double time=0.0;				//!< simulation time
	double dtFraction=0.25; 	    //!< fraction of critical time step
	double kineticEnergy=0.0;		//!< kinetic energy of the model

	string inputFile="";			//!< input file name

	StressUpdateScheme stress=StressUpdateScheme::USL; //!< current stress scheme

	DampingType damping=DampingType::NONE; //!< damping type

    #if defined (_WIN64) || defined(_WIN32)
	OperationalSystem operationalSystem=OperationalSystem::WINDOWS; //!< operational system
    #elif defined (linux) || defined(__linux__)
	OperationalSystem operationalSystem=OperationalSystem::LINUX; //!< operational system
    #endif

    InterpolationFunctionType interpolationType = InterpolationFunctionType::GIMP;
	
	bool isWindowsSystem(){
		return operationalSystem==OperationalSystem::WINDOWS?true:false;
	}

	bool isLinuxSystem(){
		return operationalSystem==OperationalSystem::LINUX?true:false;
	}

	int getContributionNodesNum(){
		return contributionNodes;
	}
}
