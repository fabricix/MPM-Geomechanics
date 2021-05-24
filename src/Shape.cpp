/*
 * Shape.cpp
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Shape.h"
#include "Utilities.h"

#include <cstdlib>
using std::abs;

Shape::Shape() {
	
	shape.setZero();
	derivate.setZero();
}

Shape::~Shape() {
	// TODO Auto-generated destructor stub
}

Vector3d Shape::getShape() const {

    return shape;
}

Vector3d Shape::getDerivate() const {

    return derivate;
}

void Shape::setShape(double sx, double sy, double sz) {

    shape=Vector3d(sx,sy,sz);
}

void Shape::setDerivate(double gx, double gy, double gz){

    derivate=Vector3d(gx,gy,gz);
}