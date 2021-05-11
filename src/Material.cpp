/*
 * Material.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;

#include "Material.h"

Material::Material(int id, double density, MaterialType type) {
	
	setDensity(density);
	setId(id);
	setType(type);
}

Material::~Material() {

}

//
// get methods
//

int Material::getId(){

	return id;
}

double Material::getDensity(){

	return density;
}

//
// set methods
//

void Material::setId(int mid){

	id=mid;
}

void Material::setDensity(double d){

	density=d;
}

void Material::setType(Material::MaterialType mt){

	type=mt;
}

Material::MaterialType Material::getType(){

	return type;
}
