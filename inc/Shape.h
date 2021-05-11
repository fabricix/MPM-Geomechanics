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
	
	virtual void update(Vector3d, Vector3d, Vector3d, Vector3d) = 0; //!< update the shape functions and its gradients

	Vector3d getShape(); //!< returns the shape function values
	Vector3d getDerivate(); //!< returns the derivates values
	
	void setShape(double,double,double); //!< configures the shape function value
	void setDerivate(double,double,double); //!< configures the derivates of the shape functions
	
private:
	
	virtual double computeGradient(double, double, double) = 0; //!< returns the gradient of the shape function
	virtual double computeShape(double, double, double) = 0; //!< returns the shape function value at a position

	Vector3d shape; //!< shape function values
	Vector3d derivate; //!< shape function gradient values
};

#endif /* SHAPE_H_ */
