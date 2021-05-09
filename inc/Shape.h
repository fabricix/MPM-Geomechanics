/*
 * Shape.h
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include "Eigen/Core"
using Eigen::Vector3d;

/// \class Shape
/// \brief Represents the shape functions used in the interpolation process.
class Shape {
public:
	
	Shape(); //!< default constructor
	virtual ~Shape(); //<! defaul destructor
	
	void updateGimp(Vector3d, Vector3d, Vector3d, Vector3d); //!< update the GIMP functions and gradients
	
	double gradientGimp(double, double, double); //!< GIMP gradient shape function
	double shapeGimp(double, double, double); //!< GIMP shape function
	
	double Sx; //!< shape function value in x
	double Sy; //!< shape function value in y
	double Sz; //!< shape function value in z

	double Gx; //!< shape function gradient value in x
	double Gy; //!< shape function gradient value in y
	double Gz; //!< shape function gradient value in z
};

#endif /* SHAPE_H_ */
