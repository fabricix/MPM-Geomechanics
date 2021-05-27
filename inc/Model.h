 /*
 * Model.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "Eigen/Core"
using Eigen::Vector3d;

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

	/// \brief Return true is the system is Windows
	/// \param[out] is_windows_system Trues if is Windows system
	bool getWindowsSystem();

	/// \brief Return true is the system is Linux
	/// \param[out] is_linux_system Trues if is Linux system
	bool getLinuxSystem();
	
	/// \brief Return the number of nodes that a particle contributes.
	int getContributionNodesNum();
	
	/// \brief Return the time step
	/// \param[out] time_step Time step
	double getTimeStep();
	
	/// \brief Configure the time step
	/// \param[in] time_step Time step
	void setTimeStep(double);

	/// \brief Return the simulation time
	/// \param[out] simulation_time Simulation time
	double getTime();
	
	/// \brief Configure the simulation time
	/// \param[in] simulation_time Simulation time
	void setTime(double);

	/// \brief Return the total results to be written
	/// \param[out] total_results Number of results
	int getResultNum();
	
	/// \brief Configure the total results to be written
	/// \param[in] total_results Number of results
	void setResultNum(int);
	
	/// \brief Return the every how many steps the results will be written
	/// \param[out] n_steps_results Number of steps
	int getResultSteps();

	/// \brief Return the fraction of time steps
	/// \param[out] time_step_fraction Time step fraction
	double getTimeStepFraction();
	
	/// \brief Configures the fraction of critical time step
	/// \param[in] time_step_fraction Time step fraction
	void setTimeStepFraction(double);

	/// \brief Return the number of threads in the simulation
	/// \param[out] n_threads Number of threads
	int getThreads();
	
	/// \brief Configures the number of threads
	/// \param[in] n_threads Number of threads
	void setThreads(int);

	/// \brief Return if contact method is activated
	/// \param[out] is_contact_active Contact activated
	bool getContactActive();
	
	/// \brief Configure if contact method is activated
	/// \param[in] is_contact_activated Contact active
	void setContactActive(bool);

	/// \brief Return if gravity is active
	/// \param[out] is_gravity_activated Gravity active
	bool getGravityActive();
	
	/// \brief Configure if gravity is active
	/// \param[in] is_gravity_activated Gravity active
	void setGravityActive(bool);

	/// \brief Return the gravity vector
	/// \param[out] gravity_vector A vector containing the gravity
	/// acceleration in each direction
	Vector3d getGravity();
	
	/// \brief Configure the gravity vector
	/// \param[in] gravity_vector A vector containing the gravity
	/// acceleration in each direction
	void setGravity(Vector3d);

	/// \brief Return if axisymetric analisys is active
	/// \param[out] if_axysimentric_active Axisymetric active value
	bool getAxisymetricActive();
	
	/// \brief Configure if axisymetric analisys is active
	/// \param[in] if_axysimentric_active Axisymetric active value
	void setAxisymetricActive(bool);

	/// \brief Return if Jaumann rate is active
	/// \param[out] if_jaumann_active Jaumann active value
	bool getJanumannActive();
	
	/// \brief Configure if Jaumann rate is active
	/// \param[in] if_jaumann_active Jaumann active value
	void setJanumannActive(bool);

	/// \brief Return the input file name
	/// \param[out] file_name File name
	string getInputFile();
	
	/// \brief Configure the input file name
	/// \param[in] file_name File name
	void setInputFile(string);

	/// \brief Return the scheme for updating the stress
	/// \param[out] stress_sheme_update Stress scheme update
	/// ModelSetup::StressUpdateScheme
	ModelSetup::StressUpdateScheme getUpdateStressScheme();
	
	/// \brief Configure the scheme for updating the stress
	/// \param[in] stress_sheme_update Stress scheme update
	/// ModelSetup::StressUpdateScheme
	void setUpdateStressScheme(ModelSetup::StressUpdateScheme);

	/// \brief Return the damping type
	/// \param[out] damping_type Damping type ModelSetup::DampingType
	ModelSetup::DampingType getDamping();
	
	/// \brief Configure the damping type
	/// \param[in] damping_type Damping type ModelSetup::DampingType
	void setDamping(ModelSetup::DampingType);

	/// \brief Return the interpolation functions type
	/// \param[out] interpolation_functions_type Interpolation
	/// functions type ModelSetup::InterpolationFunctionType
	ModelSetup::InterpolationFunctionType getInterpolationFunction();
	
	/// \brief Configure the interpolation functions type
	/// \param[in] interpolation_functions_type Interpolation
	/// functions type ModelSetup::InterpolationFunctionType
	void setInterpolationFunction(ModelSetup::InterpolationFunctionType);
};

#endif /* MODEL_H_ */
