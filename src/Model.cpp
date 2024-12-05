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
	
	// coupled hydro-mechanical analisys 
	bool twoPhaseCalculationActive=false; //!< is two-phase calculation active

	// contact
	bool contactActive=false; //!< is contact active

	// states
	bool loadState = false; //!< if state need to be loaded
	bool saveState = false; //!< if state need to be salved
	
	// gravity active
	bool gravityActive=false; //!< is gravity active

	// gravity dafault value
	Vector3d gravity(0,0,-10); //!< gravity vector
	
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
	double dt = 0.0; //!< time step
	double time = 0.0; //!< simulation time
	double dt_critical_multiplier = 0.25; //!< dafault critical time step fraction
	double currentTime = 0.0; //!< current simulation time

	// initial time simulation
    std::chrono::system_clock::time_point initialSimulationTime;

	// default damping
	DampingType damping=DampingType::UNDAMPED; //!< damping type
	
	// default local damping value
	double localDamping=0.0; //!< local damping value

	// input file
	string inputFile=""; //!< input file name

	// default stress update scheme
	StressUpdateScheme stress=StressUpdateScheme::USL; //!< current stress scheme

	// default interpolation functions
    InterpolationFunctionType interpolationType = InterpolationFunctionType::GIMP; //!< interpolation function type
	
    // seismic analysis
	bool seismicAnalysisActive = false; //!< is seismic analysis

	///
	/// Function members
	///

    // states 
	bool getLoadState() { return loadState; }
	void setLoadState(bool st) { loadState = st; }
	bool getSaveState() { return saveState; }
	void setSaveState(bool st) { saveState = st; }

	// contribution nodes
	unsigned getContributionNodesNum() { return contributionNodes; }

	// total time
	double getTime() { return time; }
	void setTime(double d) { time=d; }

	// results
	unsigned getResultNum() { return resultNumber; }
	void setResultNum(unsigned d) { resultNumber=d; }
	unsigned getResultSteps() { return static_cast<unsigned int>(floor(time/dt)/resultNumber); }

	// time step
	double getTimeStep() { return dt; }
	void setTimeStep(double d) { dt=d; }
	double getCriticalTimeStepMultiplier() { return dt_critical_multiplier; }
	void setCriticalTimeStepMultiplier(double d) { dt_critical_multiplier=d; }

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

	// coupled analysis
	bool getTwoPhaseActive() { return twoPhaseCalculationActive; }
	void setTwoPhaseActive(bool d) { twoPhaseCalculationActive=d; }

    // simulation time
    void setInitialSimulationTime(std::chrono::system_clock::time_point initialTime) { ModelSetup::initialSimulationTime = initialTime; }
    std::chrono::system_clock::time_point getInitialSimulationTime() { return ModelSetup::initialSimulationTime;  }
	double getCurrentTime() { return currentTime; }
	void setCurrentTime(double a) { currentTime = a; }

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
    
	// openMP threads
	void setNumThreads(unsigned nThreads){

		#ifdef _OPENMP
		omp_set_num_threads((nThreads>0&&nThreads<=(unsigned)omp_get_num_procs())?nThreads:omp_get_num_procs());
		#endif
	}

	// Seismic analysis
	string seismic_file_name = "base_acceleration.csv";
	string getSeismicFileName() {return seismic_file_name;}
	bool getSeismicAnalysis() {return seismicAnalysisActive;}
	void setSeismicAnalysis(bool a) {seismicAnalysisActive = a;}
}