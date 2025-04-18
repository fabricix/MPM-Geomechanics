// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef SHAPEGIMP_H_
#define SHAPEGIMP_H_

#include "Shape/Shape.h"

/// \class ShapeGimp
/// \brief Represents the GIMP shape functions used in the interpolation process.
class ShapeGimp: public Shape {

public:

	/// \brief Default constructor
	///
	ShapeGimp();
	
	/// \brief Default destructor
	///
	virtual ~ShapeGimp();
	
	/// \brief Update the shape functions and its gradients
	/// \param[in] particle_position Particle position
	/// \param[in] nodal_position Nodal position
	/// \param[in] cell_dimension Cell dimension
	/// \param[in] particle_size Particle size
	virtual void update(const Vector3d& particle_position, const Vector3d& nodal_position, const Vector3d& cell_dimension, const Vector3d& particle_size);

private:
	
	/// \brief Returns the gradient of the shape function
	/// \param[in] pI_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \return The gradient of the shape function \f$dS_{Ip}/di\f$
	virtual double computeGradient(double pI_position, double cell_dimension, double lp);
	
	/// \brief Returns the shape function value
	/// \param[in] pI_position Relative position of
	/// the particle \f$p\f$ respect to the node: \f$x_p-x_I\f$
	/// \param[in] cell_dimension Cell dimension in the direction
	/// \param[in] lp Half current particle size
	/// \return Shape function value \f$S_{Ip}\f$
	virtual double computeShape(double pI_position, double cell_dimension, double lp);
};

#endif /* SHAPEGIMP_H_ */
