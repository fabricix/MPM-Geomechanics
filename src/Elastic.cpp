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

void Elastic::setYoung(double y){

	Young=y;
}
	
void Elastic::setPoisson(double p){

	Poisson=p;
}

double Elastic::getShearModulus(){

	return Young/2.0/(1.0+Poisson);
}

double Elastic::getBulkModulus(){

	return Young/3.0/(1.0-2.0*Poisson);
}

Material::MaterialType Elastic::getType(){

	return Material::getType();
}

void Elastic::updateStress(Particle* particle){

	// strain increment and its deviate
	Matrix3d dstrain = particle->getStrainIncrement();
	Matrix3d dstrain_dev = dstrain-Matrix3d::Identity()*dstrain.trace();
	double dstrain_vol = dstrain.trace();

	// current stress and deviate
	Matrix3d stress = particle->getStress();
	Matrix3d stress_dev = stress-Matrix3d::Identity()*stress.trace();;

	// rotated matrix
	Matrix3d spin = particle->getVorticityIncrement();
	Matrix3d stress_dev_rotated = stress_dev+(stress_dev*spin.transpose()+spin*stress_dev);
	
	// new stresses
	Matrix3d stress_dev_new = stress_dev_rotated+2.0*getShearModulus()*dstrain_dev;
	double stress_mean_new = stress.trace()+getBulkModulus()*dstrain_vol;

	// sets the new stress
	particle->setStress(stress_dev_new+Matrix3d::Identity()*stress_mean_new);
}
