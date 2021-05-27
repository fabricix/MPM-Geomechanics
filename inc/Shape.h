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
	
	/// \brief Default constructor
	///
	Shape();
	
	/// \brief Default destructor
	///
	virtual ~Shape();
	
	/// \brief Update the shape functions and its gradients
	/// \param[in] particle_position Particle position
	/// \param[in] nodal_position Nodal position
	/// \param[in] cell_dimension Cell dimension
	/// \param[in] particle_size Particle size
	virtual void update(Vector3d, Vector3d, Vector3d, Vector3d) = 0;

	/// \brief Return the shape function values
	/// \param[out] shape_function_value The nodal shape 
	/// function value \f$S_{Ipx}, S_{Ipy}, S_{Ipy}\f$
	Vector3d getShape() const;
	
	/// \brief Returns the derivates values
	/// \param[out] shape_function_derivates The derivates 
	/// of the nodal shape function \f$dS_{Ip}/dx, dS_{Ip}/dy, dS_{Ip}/dz\f$
	Vector3d getDerivate() const;
	
	/// \brief Configure the shape function values
	/// \param[in] shape_function_value The nodal shape 
	/// function values \f$S_{Ipx}, S_{Ipy}, S_{Ipy}\f$
	void setShape(double,double,double);
	
	/// \brief Configure the nodal shape function derivates
	/// \param[in] shape_function_derivates The derivates 
	/// of the nodal shape function \f$dS_{Ip}/dx, dS_{Ip}/dy, dS_{Ip}/dz\f$
	void setDerivate(double,double,double);
	
private:
	
	/// \brief Returns the gradient of the shape function
	/// \param[in] p_i_relative_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \param[out] \f$dS_{Ip}/di\f$
	virtual double computeGradient(double, double, double) = 0;
	
	/// \brief Returns the shape function value
	/// \param[in] p_I_relative_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \param[out] \f$S_{Ip}\f$
	virtual double computeShape(double, double, double) = 0;

	Vector3d shape; //!< shape function values
	
	Vector3d derivate; //!< shape function gradient values
};

#endif /* SHAPE_H_ */
