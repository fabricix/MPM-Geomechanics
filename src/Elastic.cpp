/*
 * Elastic.cpp
 *
 *  Created on: 4 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

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
