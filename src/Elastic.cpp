// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <cmath>
using std::sqrt;

#include "Materials/Elastic.h"

void Elastic::updateStress(Particle* particle) const {

	// strain increment and its deviate
	Matrix3d de = particle->getStrainIncrement();
	Matrix3d deDev = de-Matrix3d::Identity()*de.trace()/3.0;
	double deVol = de.trace();

	// current stress and deviate
	Matrix3d stress = particle->getStress();
	Matrix3d stressDev = stress-Matrix3d::Identity()*stress.trace()/3.0;
	double stressMean = stress.trace()/3.0;

	// integrate the stresses
	Matrix3d stressDevNew = stressDev+2.0*getShearModulus()*deDev;
	double stressMeanNew = stressMean+getBulkModulus()*deVol;

	// sets the new stress
	particle->setStress(stressDevNew+Matrix3d::Identity()*stressMeanNew);
}

double Elastic::getSoundSpeed( ) const {

	// sound speed
	return sqrt(Young*(1.0-Poisson)/(1.0+Poisson)/(1.0-2.0*Poisson)/density);
}
