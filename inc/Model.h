/*
 * Model.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MODEL_H_
#define MODEL_H_

#include<string>
using std::string;

namespace ModelSetup{
	
	enum StressUpdateScheme{USF, USL, MUSL};
	enum DampingType{NONE,LOCAL_NO_VISCOUS,DYNAMIC_RELAX};
	enum OperationalSystem{WINDOWS,LINUX};
}

class Model {

public:

	Model();
	virtual ~Model();

	bool isWindowsSystem();
	bool isLinuxSystem();

	bool contact;				//!< is contact active
	bool gravity;				//!< is gravity active
	bool axisymetric;			//!< is axisymetric model
	bool coupled;				//!< is coupled analysis
	bool jaumann;				//!< is Jaumann rate active

	int istep;					//!< current time step
	int nThreads;				//!< number of threads in current job

	double dt;					//!< time step
	double time;				//!< simulation time
	double dtFraction; 		    //!< fraction of critical time step
	double kineticEnergy;		//!< kinetic energy of the model

	string inputFile;			//!< input file name

	ModelSetup::OperationalSystem operationalSystem; //!< operational system

	ModelSetup::StressUpdateScheme stressScheme; //!< current stress scheme

	ModelSetup::DampingType damping; //!< damping type

};

#endif /* MODEL_H_ */
