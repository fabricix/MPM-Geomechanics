/*
 * Model.cpp
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Model.h"

#include <cmath>
using std::floor;

#include <omp.h>

namespace ModelSetup {

	///
	/// Data members
	///

	// contact
	bool contactActive=false; //!< is contact active
	
	// gravity
	bool gravityActive=false; //!< is gravity active
	Vector3d gravity(0,0,-9.81); //!< gravity vector
	
	// axisymmetric model
	bool axisymetricActive=false; //!< is axisymmetric model
	
	// Jaumann stress rate 
	bool jaumannActive=true; //!< is Jaumann rate active
	
	// threads
	unsigned nThreads=1; //!< number of threads in current job
	
	// contribution nodes
	unsigned contributionNodes=27; //!< nodes that the particles contributed
	
	// results
	unsigned resultNumber=10; //!< number of results to write

	// time
	double dt=0.0; //!< time step
	double time=0.0; //!< simulation time
	double dtFraction=0.25; //!< fraction of critical time step

	// damping
	DampingType damping=DampingType::UNDAMPED; //!< damping type
	double localDamping=0.0; //!< local damping value

	// input file
	string inputFile=""; //!< input file name

	// stress update scheme
	StressUpdateScheme stress=StressUpdateScheme::USL; //!< current stress scheme

	// operational system
    #if defined (_WIN64) || defined(_WIN32)
	OperationalSystem operationalSystem=OperationalSystem::WINDOWS; //!< operational system
    #elif defined (linux) || defined(__linux__)
	OperationalSystem operationalSystem=OperationalSystem::LINUX; //!< operational system
    #endif

	// interpolation function
    InterpolationFunctionType interpolationType = InterpolationFunctionType::GIMP; //!< interpolation function type
	
	///
	/// Function members
	///

	// operational system
	bool getWindowsSystem() { return operationalSystem==OperationalSystem::WINDOWS?true:false; }
	bool getLinuxSystem() { return operationalSystem==OperationalSystem::LINUX?true:false; }
		
	// contribution nodes
	unsigned getContributionNodesNum() { return contributionNodes; }


	// total time
	double getTime() { return time; }
	void setTime(double d) { time=d; }

	// results
	unsigned getResultNum() { return resultNumber; }
	void
	setResultNum(unsigned d) { resultNumber=d; }
	unsigned getResultSteps() { return floor(time/dt)/resultNumber; }

	// time step
	double getTimeStep() { return dt; }
	void setTimeStep(double d) { dt=d; }
	double getTimeStepFraction() { return dtFraction; }
	void setTimeStepFraction(double d) { dtFraction=d; }

	// threads
	unsigned getThreads() { return nThreads; }
	void setThreads(unsigned d) { nThreads=d; }

	// contact method
	bool getContactActive() { return contactActive; }
	void setContactActive(bool d) { contactActive=d; }

	// gravity
	bool getGravityActive() { return gravityActive; }
	void setGravityActive(bool d) { gravityActive=d; }
	const Vector3d& getGravity() { return gravity; }

	void setGravity(const Vector3d& gravity_in) {
		
		gravity=gravity_in; 
		setGravityActive((gravity.x()!=0.0||gravity.y()!=0.0||gravity.z()!=0.0)?true:false);
	}

	// axisymmetric analisys
	bool getAxisymetricActive() { return axisymetricActive; }
	void setAxisymetricActive(bool d) { axisymetricActive=d; }

	// Jaumann rate
	bool getJanumannActive() { return jaumannActive; }
	void setJanumannActive(bool d) { jaumannActive=d; }

	// input file
	string getInputFile() { return inputFile; }
	void setInputFile(string d) { inputFile=d; }

	// stress update scheme
	ModelSetup::StressUpdateScheme getUpdateStressScheme() { return stress; }
	void setUpdateStressScheme(ModelSetup::StressUpdateScheme d) { stress=d; }

	// damping
	ModelSetup::DampingType getDampingType() { return damping; }
	void setDampingType(ModelSetup::DampingType d) { damping=d; }
	double getDampingLocal() { return localDamping; }
	void setDampingLocalValue(double dampingValue) {localDamping = dampingValue; }

	// interpolation functions
	ModelSetup::InterpolationFunctionType getInterpolationFunction() { return interpolationType; }
	void setInterpolationFunction(ModelSetup::InterpolationFunctionType d) { interpolationType=d; }

	void setNumThreads(unsigned nThreads) {

		#ifdef _OPENMP
		omp_set_num_threads((nThreads>0&&nThreads<=omp_get_num_procs())?nThreads:omp_get_num_procs());
		#endif
	}
}
