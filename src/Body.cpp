/*
 * Body.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Body.h"

int Body::totalBodies=0; // initialize the static member

Body::Body() {
	
	totalBodies++;
}

Body::~Body() {
	
	totalBodies--;
}

int Body::getTotalBodies(){

	return totalBodies;
}