/*
 * Elastic.cpp
 *
 *  Created on: 4 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;

#include "Elastic.h"

Elastic::Elastic(int id, double density, double Young, double Poisson)
: Material(id, density, Material::MaterialType::ELASTIC)
{
	setYoung(Young);
	setPoisson(Poisson);
}

Elastic::~Elastic() {
	// TODO Auto-generated destructor stub
}

Material::MaterialType Elastic::getType(){

	return Material::getType();
}

void Elastic::updateStress(Particle* particle){

	// strain increment and its deviate
	Matrix3d de = particle->getStrainIncrement();
	Matrix3d deDev = de-Matrix3d::Identity()*de.trace()/3.0;
	double deVol = de.trace();

	// current stress and deviate
	Matrix3d stress = particle->getStress();
	Matrix3d stressDev = stress-Matrix3d::Identity()*stress.trace()/3.0;
	double stressMean = stress.trace()/3.0;

#if 1
	// rotated matrix
	Matrix3d spin = particle->getVorticityIncrement();
	Matrix3d stressDevRot = stressDev+(stressDev*spin.transpose()+spin*stressDev);
	
	// new stresses
	Matrix3d stressDevNew = stressDevRot+2.0*getShearModulus()*deDev;
#else
	Matrix3d stressDevNew = stressDev+2.0*getShearModulus()*deDev;
#endif
	
	double stressMeanNew = stressMean+getBulkModulus()*deVol;

	// sets the new stress
	particle->setStress(stressDevNew+Matrix3d::Identity()*stressMeanNew);
}
