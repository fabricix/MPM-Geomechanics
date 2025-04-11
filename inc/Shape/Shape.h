// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

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
	virtual void update(const Vector3d& particle_position, const Vector3d& nodal_position, const Vector3d& cell_dimension, const Vector3d& particle_size) = 0;

	/// \brief Return the shape function values
	/// \return The nodal shape 
	/// function value \f$S_{Ipx}, S_{Ipy}, S_{Ipy}\f$
	inline const Vector3d& getShape() const { return this->shape; }
	
	/// \brief Returns the derivates values
	/// \return The derivates of the nodal shape
	/// function \f$dS_{Ip}/dx, dS_{Ip}/dy, dS_{Ip}/dz\f$
	inline const Vector3d& getDerivate() const { return this->derivate; }
	
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
	/// \return The gradient of the shape function: \f$dS_{Ip}/di\f$
	virtual double computeGradient(double pI_position, double cell_dimension, double lp) = 0;
	
	/// \brief Returns the shape function value
	/// \param[in] pI_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \return The shape function value \f$S_{Ip}\f$
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
