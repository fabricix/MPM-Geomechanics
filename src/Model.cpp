/*
 * Model.cpp
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Model.h"

#include <cmath>
using std::floor;

namespace ModelSetup {

	namespace  {
		
		bool contactActive=false;		//!< is contact active
		bool gravityActive=false;		//!< is gravity active
		bool axisymetricActive=false;	//!< is axisymetric model
		bool jaumannActive=true;		//!< is Jaumann rate active
		
		Vector3d gravity(0,0,-9.81);	//!< gravity vector

		int nThreads=1;					//!< number of threads in current job
		int contributionNodes=27;		//!< nodes that the particles contributed

		double dt=0.0;					//!< time step
		double time=0.0;				//!< simulation time
		double dtFraction=0.25; 	    //!< fraction of critical time step

		int resultNumber=10;			//!< number of results to write

		string inputFile="";			//!< input file name

		StressUpdateScheme stress=StressUpdateScheme::USL; //!< current stress scheme

		DampingType damping=DampingType::NONE; //!< damping type
	}

    #if defined (_WIN64) || defined(_WIN32)
	OperationalSystem operationalSystem=OperationalSystem::WINDOWS; //!< operational system
    #elif defined (linux) || defined(__linux__)
	OperationalSystem operationalSystem=OperationalSystem::LINUX; //!< operational system
    #endif

    InterpolationFunctionType interpolationType = InterpolationFunctionType::GIMP;
	
	bool getWindowsSystem(){return operationalSystem==OperationalSystem::WINDOWS?true:false; }
	bool getLinuxSystem(){ return operationalSystem==OperationalSystem::LINUX?true:false; }
	
	int getContributionNodesNum(){ return contributionNodes; }

	double getTime(){ return time; }
	void setTime(double d){ time=d; }

	int getResultNum(){ return resultNumber; }
	void setResultNum(int d){ resultNumber=d; }

	int getResultSteps(){ return floor(time/dt)/resultNumber; }

	double getTimeStep(){ return dt; }
	void setTimeStep(double d){ dt=d; }

	double getTimeStepFraction(){ return dtFraction; }
	void setTimeStepFraction(double d){ dtFraction=d; }

	int getThreads(){ return nThreads; }
	void setThreads(int d){ nThreads=d; }

	bool getContactActive(){ return contactActive; }
	void setContactActive(bool d){ contactActive=d; }

	bool getGravityActive(){ return gravityActive; }
	void setGravityActive(bool d){ gravityActive=d; }

	Vector3d getGravity(){ return gravity; }
	
	void setGravity(Vector3d d){ 
		
		gravity=d; 
		setGravityActive((gravity.x()!=0.0||gravity.y()!=0.0||gravity.z()!=0.0)?true:false);
	}

	bool getAxisymetricActive(){ return axisymetricActive; }
	void setAxisymetricActive(bool d){ axisymetricActive=d; }

	bool getJanumannActive(){ return jaumannActive; }
	void setJanumannActive(bool d){ jaumannActive=d; }

	string getInputFile(){ return inputFile; }
	void setInputFile(string d){ inputFile=d; }

	ModelSetup::StressUpdateScheme getUpdateStressScheme(){ return stress; }
	void setUpdateStressScheme(ModelSetup::StressUpdateScheme d){ stress=d; }

	ModelSetup::DampingType getDamping(){ return damping; }
	void setDamping(ModelSetup::DampingType d){ damping=d; }

	ModelSetup::InterpolationFunctionType getInterpolationFunction(){ return interpolationType; }
	void setInterpolationFunction(ModelSetup::InterpolationFunctionType d){ interpolationType=d; }
}
