/*
 * Elastic.cpp
 *
 *  Created on: 4 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;

#include "Elastic.h"

void Elastic::updateStress(Particle* particle){

	// strain increment and its deviate
	Matrix3d de = particle->getStrainIncrement();
	Matrix3d deDev = de-Matrix3d::Identity()*de.trace()/3.0;
	double deVol = de.trace();

	// current stress and deviate
	Matrix3d stress = particle->getStress();
	Matrix3d stressDev = stress-Matrix3d::Identity()*stress.trace()/3.0;
	double stressMean = stress.trace()/3.0;

	Matrix3d stressDevNew;

	if (this->jaumann){

		// rotated matrix
		Matrix3d spin = particle->getVorticityIncrement();
		Matrix3d stressDevRot = stressDev+(stressDev*spin.transpose()+spin*stressDev);
		
		// new stresses
		stressDevNew = stressDevRot+2.0*getShearModulus()*deDev;
	
	}
	else{
		stressDevNew = stressDev+2.0*getShearModulus()*deDev;
	}
	
	double stressMeanNew = stressMean+getBulkModulus()*deVol;

	// sets the new stress
	particle->setStress(stressDevNew+Matrix3d::Identity()*stressMeanNew);
}
