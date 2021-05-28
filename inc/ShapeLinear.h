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
	
	/// \brief Default constructor
	///
	ShapeLinear();
	
	/// \brief Default desconstructor
	///
	virtual ~ShapeLinear();
	
	/// \brief Update the shape functions and its gradients
	/// \param[in] particle_position Particle position
	/// \param[in] nodal_position Nodal position
	/// \param[in] cell_dimension Cell dimension
	/// \param[in] particle_size Particle size
	virtual void update(Vector3d, Vector3d, Vector3d, Vector3d);

private:

	/// \brief Returns the gradient of the shape function
	/// \param[in] p_i_relative_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \param[out] \f$dS_{Ip}/di\f$
	virtual double computeGradient(double, double, double);

	/// \brief Returns the shape function value
	/// \param[in] p_I_relative_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \param[out] \f$S_{Ip}\f$
	virtual double computeShape(double, double, double);
};

#endif /* SHAPELINEAR_H_ */