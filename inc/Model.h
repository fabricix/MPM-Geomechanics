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

/// \namespace ModelSetup
/// \brief Contain the configuration variables that defines an specific simulation. 
namespace ModelSetup {

	/// \enum StressUpdateScheme
	/// \brief Defines the type of scheme used to update the stress.
	enum StressUpdateScheme{USF, USL, MUSL};

	/// \enum DampingType
	/// \brief Defines the type of damping.
	enum DampingType{NONE,LOCAL_NO_VISCOUS,DYNAMIC_RELAX};
	
	/// \enum OperationalSystem
	/// \brief Defines the current operational system.
	enum OperationalSystem{WINDOWS,LINUX};

	/// \enum InterpolationFunctionType
	/// \brief Defines the type of interpolation function.
	enum InterpolationFunctionType{LINEAR, GIMP};

	bool getWindowsSystem(); //<! returns true is the system is Windows
	bool getLinuxSystem(); //<! returns true if the system is Linux
	
	int getContributionNodesNum(); //<! returns the number of nodes that a particle contributes.
	
	double getDt(); //<! returns the time step
	void setDt(double); //<! configures the time step

	double getTime(); //<! returns the simulation time
	void setTime(double); //<! configures the simulation time

	int getResultNum(); //!< returns the total results to be written
	void setResultNum(int); //!< configures the total results
	int getResultSteps(); //!< returns the step when the results will be written

	double getDtFraction(); //<! returns
	void setDtFraction(double); //<! configures the fraction of critical time step

	int getThreads(); //<! returns
	void setThreads(int); //<! configures the number of threads

	bool getContact(); //<! returns if contact is active
	void setContact(bool); //<! configures if contact is active

	bool getGravity(); //<! returns if gravity is active
	void setGravity(bool); //<! configures if gravity is active

	bool getAxisymetric(); //<! returns if axisymetric analisys is active
	void setAxisymetric(bool); //<! configures if axisymetric analisys is active

	bool getJanumann(); //<! returns if Jaumann rate is active
	void setJanumann(bool); //<! configures if Jaumann rate is active

	string getInputFile(); //<! returns the input file name
	void setInputFile(string); //<! configures the input file name

	ModelSetup::StressUpdateScheme getUpdateStressScheme(); //<! returns the scheme for updating the stress
	void setUpdateStressScheme(ModelSetup::StressUpdateScheme); //<! configures the scheme for updating the stress

	ModelSetup::DampingType getDamping(); //<! returns the damping
	void setDamping(ModelSetup::DampingType); //<! configures the damping type

	ModelSetup::InterpolationFunctionType getInterpolationFunction(); //<! returns the interpolation functions
	void setInterpolationFunction(ModelSetup::InterpolationFunctionType); //<! configures the interpolation functions
};

#endif /* MODEL_H_ */
