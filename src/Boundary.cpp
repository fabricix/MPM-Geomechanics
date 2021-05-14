/*
 * Boundary.cpp
 *
 *  Created on: 27 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Boundary.h"

Boundary::Boundary(){

	planeX0.type=BoundaryType::SLIDING;
	planeY0.type=BoundaryType::SLIDING;
	planeZ0.type=BoundaryType::SLIDING;
	planeXn.type=BoundaryType::SLIDING;
	planeYn.type=BoundaryType::SLIDING;
	planeZn.type=BoundaryType::SLIDING;
}

Boundary::~Boundary() {
	// TODO Auto-generated destructor stub
}

const Boundary::planeBoundary* Boundary::getPlaneX0(){ return &planeX0; }
const Boundary::planeBoundary* Boundary::getPlaneY0(){ return &planeY0; }
const Boundary::planeBoundary* Boundary::getPlaneZ0(){ return &planeZ0; }
const Boundary::planeBoundary* Boundary::getPlaneXn(){ return &planeXn; }
const Boundary::planeBoundary* Boundary::getPlaneYn(){ return &planeYn; }
const Boundary::planeBoundary* Boundary::getPlaneZn(){ return &planeZn; }

void Boundary::setNodesPlaneX0(vector<int> nodes){ planeX0.nodes=nodes; }
void Boundary::setNodesPlaneY0(vector<int> nodes){ planeY0.nodes=nodes; }
void Boundary::setNodesPlaneZ0(vector<int> nodes){ planeZ0.nodes=nodes; }
void Boundary::setNodesPlaneXn(vector<int> nodes){ planeXn.nodes=nodes; }
void Boundary::setNodesPlaneYn(vector<int> nodes){ planeYn.nodes=nodes; }
void Boundary::setNodesPlaneZn(vector<int> nodes){ planeZn.nodes=nodes; }