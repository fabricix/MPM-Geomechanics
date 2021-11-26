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
/// \brief Contain the configuration variables that defines an specific simulation
namespace ModelSetup {

	/// \enum StressUpdateScheme
	/// \brief Defines the type of scheme used to update the stress
	enum StressUpdateScheme{ USF, USL, MUSL };

	/// \enum DampingType
	/// \brief Defines the type of damping
	enum DampingType{ UNDAMPED, LOCAL, RAYLEIGH, ARTIFICIAL_VISCOSITY, KINETIC_DYNAMIC_RELAXATION };
	
	/// \enum OperationalSystem
	/// \brief Defines the current operational system
	enum OperationalSystem{ WINDOWS, LINUX };

	/// \enum InterpolationFunctionType
	/// \brief Defines the type of interpolation function
	enum InterpolationFunctionType{ LINEAR, GIMP };

	/// \brief Return true is the system is Windows
	/// \return True if is Windows system
	bool getWindowsSystem();

	/// \brief Return true is the system is Linux
	/// \return True if is Linux system
	bool getLinuxSystem();
	
	/// \brief Return the number of nodes that a particle contributes
	/// \return The number of nodes that a particle contributes
	unsigned getContributionNodesNum();
	
	/// \brief Return the time step
	/// \return Time step
	double getTimeStep();
	
	/// \brief Configure the time step
	/// \param[in] time_step Time step
	void setTimeStep(double time_step);

	/// \brief Return the simulation time
	/// \return Simulation time
	double getTime();
	
	/// \brief Configure the simulation time
	/// \param[in] simulation_time Simulation time
	void setTime(double simulation_time);

	/// \brief Return the total results to be written
	/// \return Number of results
	unsigned getResultNum();
	
	/// \brief Configure the total results to be written
	/// \param[in] total_results Number of results
	void setResultNum(unsigned total_results);
	
	/// \brief Return the every how many steps the results will be written
	/// \return Number of steps
	unsigned getResultSteps();

	/// \brief Return the fraction of time steps
	/// \return Time step fraction
	double getTimeStepFraction();
	
	/// \brief Configures the fraction of critical time step
	/// \param[in] time_step_fraction Time step fraction
	void setTimeStepFraction(double time_step_fraction);

	/// \brief Return the number of threads in the simulation
	/// \return Number of threads
	unsigned getThreads();
	
	/// \brief Configures the number of threads
	/// \param[in] n_threads Number of threads
	void setThreads(unsigned n_threads);

	/// \brief Return if contact method is activated
	/// \return True if contact method is activated
	bool getContactActive();
	
	/// \brief Configure if contact method is activated
	/// \param[in] is_contact_activated Contact active
	void setContactActive(bool is_contact_activated);

	/// \brief Return if gravity is active
	/// \return True if gravity active
	bool getGravityActive();
	
	/// \brief Configure if gravity is active
	/// \param[in] is_gravity_activated Gravity active
	void setGravityActive(bool is_gravity_activated);

	/// \brief Return the gravity vector
	/// \return A vector containing the gravity
	/// acceleration in each direction
	const Vector3d& getGravity();
	
	/// \brief Configure the gravity vector
	/// \param[in] gravity A vector containing the gravity
	/// acceleration in each direction
	void setGravity(const Vector3d& gravity);

	/// \brief Return if axisymetric analisys is active
	/// \return True is axisymetric condition is active
	bool getAxisymetricActive();
	
	/// \brief Configure if axisymetric analisys is active
	/// \param[in] if_axysimentric_active Axisymetric active value
	void setAxisymetricActive(bool if_axysimentric_active);

	/// \brief Return if Jaumann rate is active
	/// \return True if Jaumann rate correction is active
	bool getJanumannActive();
	
	/// \brief Configure if Jaumann rate is active
	/// \param[in] if_jaumann_active Jaumann active value
	void setJanumannActive(bool if_jaumann_active);

	/// \brief Return the input file name
	/// \return File name
	string getInputFile();
	
	/// \brief Configure the input file name
	/// \param[in] file_name File name
	void setInputFile(string file_name);

	/// \brief Return the scheme for updating the stress
	/// \return Stress scheme update
	/// ModelSetup::StressUpdateScheme
	ModelSetup::StressUpdateScheme getUpdateStressScheme();
	
	/// \brief Configure the scheme for updating the stress
	/// \param[in] stress_sheme_update Stress scheme update
	/// ModelSetup::StressUpdateScheme
	void setUpdateStressScheme(ModelSetup::StressUpdateScheme stress_sheme_update);

	/// \brief Return the damping type
	/// \return Damping type ModelSetup::DampingType
	ModelSetup::DampingType getDamping();
	
	/// \brief Return the local damping value
	/// \return Alpha The value for the local damping
	double getDampingLocal();

	/// \brief Configure the local damping value
	/// \param[in] alpha Local damping value
	void setDampingLocalValue(double alpha);

	/// \brief Configure the damping type
	/// \param[in] dampingType Damping type ModelSetup::DampingType
	void setDampingType(ModelSetup::DampingType dampingType);
	
	/// \brief Return the damping type
	/// \return dampingType Damping type ModelSetup::DampingType
	ModelSetup::DampingType getDampingType();

	/// \brief Return the interpolation functions type
	/// \return interpolation_functions_type Interpolation
	/// functions type ModelSetup::InterpolationFunctionType
	ModelSetup::InterpolationFunctionType getInterpolationFunction();
	
	/// \brief Configure the interpolation functions type
	/// \param[in] interpolation_functions_type Interpolation
	/// functions type ModelSetup::InterpolationFunctionType
	void setInterpolationFunction(ModelSetup::InterpolationFunctionType interpolation_functions_type);
	
	/// \brief Configure the number of threads in the model
	/// \param[in] nThreads Number of threads
	void setNumThreads(unsigned nThreads);

	/// \brief Return if two-phase calculation is active
	/// \return True if two-phase calculation is active
	bool getTwoPhaseActive();

	/// \brief Configure if two-phase calculation is active
	/// \param[in] two_phase_calculation_active 
	void setTwoPhaseActive(bool two_phase_calculation_active);
};

#endif /* MODEL_H_ */