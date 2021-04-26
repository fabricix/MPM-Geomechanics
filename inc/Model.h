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

namespace ModelSetup {

	enum StressUpdateScheme{USF, USL, MUSL};
	enum DampingType{NONE,LOCAL_NO_VISCOUS,DYNAMIC_RELAX};
	enum OperationalSystem{WINDOWS,LINUX};
	enum InterpolationFunctionType{LINEAR, GIMP};

	bool isWindowsSystem();
	bool isLinuxSystem();

	int getContributionNodesNum();
};

#endif /* MODEL_H_ */
