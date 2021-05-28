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
	virtual void update(Vector3d particle_position, Vector3d nodal_position, Vector3d cell_dimension, Vector3d particle_size) = 0;

	/// \brief Return the shape function values
	/// \param[out] shape_function_value The nodal shape 
	/// function value \f$S_{Ipx}, S_{Ipy}, S_{Ipy}\f$
	inline Vector3d getShape() const { return this->shape; }
	
	/// \brief Returns the derivates values
	/// \param[out] shape_function_derivates The derivates 
	/// of the nodal shape function \f$dS_{Ip}/dx, dS_{Ip}/dy, dS_{Ip}/dz\f$
	inline Vector3d getDerivate() const { return this->derivate; }
	
	/// \brief Configure the shape function values
	///
	/// Nodal shape function values: \f$S_{Ipx}, S_{Ipy}, S_{Ipy}\f$
	/// \param[in] sx The nodal shape function value in X
	/// \param[in] sy The nodal shape function value in Y
	/// \param[in] sz The nodal shape function value in Z
	inline void setShape(double sx, double sy, double sz) { this->shape=Vector3d(sx,sy,sz); }
	
	/// \brief Configure the nodal shape function derivates
	///
	/// The nodal derivate shape function values: \f$dS_{Ip}/dx, dS_{Ip}/dy, dS_{Ip}/dz\f$
	/// \param[in] gx The nodal derivate shape function value in X
	/// \param[in] gy The nodal derivate shape function value in Y
	/// \param[in] gz The nodal derivate shape function value in Z
	inline void setDerivate(double gx, double gy, double gz) { this->derivate=Vector3d(gx,gy,gz); }
	
private:
	
	/// \brief Returns the gradient of the shape function
	/// \param[in] pI_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \param[out] \f$dS_{Ip}/di\f$
	virtual double computeGradient(double pI_position, double cell_dimension, double lp) = 0;
	
	/// \brief Returns the shape function value
	/// \param[in] pI_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \param[out] \f$S_{Ip}\f$
	virtual double computeShape(double pI_position, double cell_dimension, double lp) = 0;

	Vector3d shape; //!< shape function values
	
	Vector3d derivate; //!< shape function gradient values
};

inline Shape::Shape() {
	
	this->shape.setZero();
	this->derivate.setZero();
}

inline Shape::~Shape() {
	
}

#endif /* SHAPE_H_ */