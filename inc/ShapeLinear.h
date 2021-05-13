/*
 * ShapeLinear.h
 *
 *  Created on: 8 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SHAPELINEAR_H_
#define SHAPELINEAR_H_

#include <Shape.h>

/// \class ShapeLinear
/// \brief Represents the Linear shape functions used in the interpolation process.
class ShapeLinear: public Shape {

public:
	
	ShapeLinear(); //!< default constructor
	virtual ~ShapeLinear(); //!< default destructor
	virtual void update(Vector3d, Vector3d, Vector3d, Vector3d); //!< update the shape functions and its gradients

private:

	virtual double computeGradient(double, double, double); //!< returns the gradient of the shape function
	virtual double computeShape(double, double, double); //!< returns the shape function value at a position
};

#endif /* SHAPELINEAR_H_ */
