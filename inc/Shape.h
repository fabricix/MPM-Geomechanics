/*
 * Shape.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include "../inc/Eigen/Core"
using Eigen::Vector3d;

enum InterpolationFunctionType{LINEAR, GIMP};

class Shape {
public:
	
	Shape();
	virtual ~Shape();
	
	void updateGimp(Vector3d, Vector3d, Vector3d, Vector3d); //!< update the GIMP functions and gradients
	void updateGimpZZ(Vector3d, Vector3d, Vector3d, Vector3d); //!< update the GIMP functions and gradients

public:
	
	double gradientGimp(double, double, double); //!< GIMP gradient shape function
	double shapeGimp(double, double, double); //!< GIMP shape function
	
	double gradientGimpZZ(double, double, double); //!< GIMP gradient shape function
	double shapeGimpZZ(double, double, double); //!< GIMP shape function

	double Sx; //!< shape function value in x
	double Sy; //!< shape function value in y
	double Sz; //!< shape function value in z

	double Gx; //!< shape function gradient value in x
	double Gy; //!< shape function gradient value in y
	double Gz; //!< shape function gradient value in z
};

#endif /* SHAPE_H_ */
