/*
 * Elastic.cpp
 *
 *  Created on: 4 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

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

double Elastic::getCompressionalWaveSpeed() const {
	// compresional wave
	return sqrt(Young * (1.0 - Poisson) / ((1.0 + Poisson) * (1.0 - 2.0 * Poisson) * density));
}

double Elastic::getShearWaveSpeed() const {
	// shear modulus
	double shearModulus = Young / (2.0 * (1.0 + Poisson));
	// shear wave velocity
	return sqrt(shearModulus / density);
}