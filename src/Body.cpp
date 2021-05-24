/*
 * Body.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include<string>
using std::to_string;

#include <vector>

#include <cmath>
using std::pow;

#include "Eigen/Core"
using Eigen::Vector3d;
using Eigen::Vector3i;

#include "Body.h"
#include "Warning.h"

//
// static members
//
int Body::totalBodies=0; //!< initialize total materials

//
// public methods
//

Body::Body() {
	
	setId(totalBodies);

	totalBodies++;
}

Body::~Body() {
	
	totalBodies--;
}

//
// set methods
//

void Body::setId(int bid){

	id=bid;
}

void Body::setParticles(vector<Particle*> prts){

	particles=prts;
}

void Body::insertParticles(vector<Particle*> prts){

	particles.insert(particles.end(),prts.begin(),prts.end());
}

// 
// get methods
//

int Body::getTotalBodies() {

	return totalBodies;
}

int Body::getId() const {

	return id;
}

vector<Particle*>& Body::getParticles(){

	return particles;
}
