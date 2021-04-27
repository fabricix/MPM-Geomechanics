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

	bool isWindowsSystem(); //<! returns true is the system is Windows
	bool isLinuxSystem(); //<! returns true if the system is Linux

	int getContributionNodesNum(); //<! returns the number of nodes that a particle contributes.
};

#endif /* MODEL_H_ */
