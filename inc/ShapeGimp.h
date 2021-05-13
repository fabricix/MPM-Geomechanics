/*
 * ShapeGimp.h
 *
 *  Created on: 8 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef SHAPEGIMP_H_
#define SHAPEGIMP_H_

#include <Shape.h>

/// \class ShapeGimp
/// \brief Represents the GIMP shape functions used in the interpolation process.
class ShapeGimp: public Shape {

public:

	ShapeGimp(); //!< default constructor
	virtual ~ShapeGimp(); //!< default destructor
	virtual void update(Vector3d, Vector3d, Vector3d, Vector3d); //!< update the shape functions and its gradients

private:
	
	virtual double computeGradient(double, double, double); //!< returns the gradient of the shape function
	virtual double computeShape(double, double, double); //!< returns the shape function value at a position
};

#endif /* SHAPEGIMP_H_ */
