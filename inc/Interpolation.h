// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INTERPOLATION_H_
#define INTERPOLATION_H_

#include "Eigen/Core"
using Eigen::Vector3d;

#include <vector>
using std::vector;

#include "TerrainContact.h"
#include "Particle/Particle.h"

class Mesh;
class Body;
class Particle;

/// \namespace Interpolation
/// \brief Represents the interpolation operations in the MPM.
///
/// This class allows to interpolate quantities from particles to node and
/// from nodes to particle back.
namespace Interpolation
{

	/// \brief For test only
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] particles A list of particles
	void nodalMassWithParticles(Mesh *mesh, vector<Particle *> *particles);

	/// \brief Interpolate solid mass from particles to nodes
	///
	/// \f$ m_I = \sum_p m_p N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list of Bodies
	void nodalMass(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate fluid mass from particles to nodes
	///
	/// \f$ m_I^f = \sum_p m_p^f N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list of Bodies
	void nodalMassFuid(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate solid momentum from particles to nodes
	///
	/// \f$ p_{iI} = \sum_p p_{ip} N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalMomentum(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate fluid momentum from particles to nodes
	///
	/// \f$ p_{iI}^f = \sum_p p_{ip}^f N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalMomentumFluid(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate internal force of solid from particles to nodes
	///
	/// In one phase calculation:
	/// \f$ f_{iI}^{int}=-\sum_p \sigma_{ijp} V_p N_{Ip,i}\f$
	///
	/// In two phase calculation:
	/// \f$ f_{iI}^{int,s}=-\sum_p \sigma_{ijp}' N_{Ip,i} V_p + \sum_p p^f N_{Ip,i} V_p\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalInternalForce(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate internal force of fluid from particles to nodes
	///
	/// \f$ f_{iI}^{int,f}= \sum_p n p^f N_{Ip,i} V_p\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalInternalForceFluid(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate external force of solid from particles to nodes
	///
	/// In one phase calculations:
	/// \f$ f_{iI}^{ext} = \sum_p b_{ip} m_p N_{Ip} \f$
	///
	/// In two phase calculations:
	/// \f$ f_{iI}^{ext,s} = \sum_p b_{ip} m_p^f N_{Ip} + \sum_p b_{ip} m_p^s N_{Ip} \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalExternalForce(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate external force of fluid from particles to nodes
	///
	/// \f$ f_{iI}^{ext,f}=\sum_p b_{ip} m_p^w N_{Ip} - \sum_p \frac{m_p^f n g}{k_{ijp}}(v^w_{jp}-v^s_{jp})N_{Ip} \f$
	///
	/// At the moment, only the principal values of the hydraulic conductivity in 3D are considered, then:
	///
	/// \f$ f_{iI}^{ext,f}=\sum_p b_{ip} m_p^w N_{Ip} - \sum_p \frac{m_p^f n g}{k_{ip}}(v^w_{ip}-v^s_{ip})N_{Ip} \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalExternalForceFluid(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate drag force of fluid from particles to nodes
	///
	/// \f$ f_{iI}^{drag,f}= \sum_p \frac{m_p^f n g}{k_{ijp}}(v^w_{jp}-v^s_{jp})N_{Ip} \f$
	///
	/// At the moment, only the principal values of the hydraulic conductivity in 3D are considered, then:
	///
	/// \f$ f_{iI}^{drag,f}= \sum_p \frac{m_p^f n g}{k_{ip}}(v^w_{ip}-v^s_{ip})N_{Ip} \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalDragForceFluid(Mesh *mesh, vector<Body *> *bodies);

	/// \brief Interpolate the strain increment of solid at particle
	///
	/// \f$ \Delta \epsilon_{ijp} = \frac{1}{2}(N_{Ip,j} v_{iI} + N_{Ip,i} v_{jI}) \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleStrainIncrement(Mesh *mesh, vector<Body *> *bodies, double time_step);

	/// \brief Interpolate the strain increment of fluid at particle
	///
	/// \f$ \Delta \epsilon_{ijp}^f = \frac{1}{2}(N_{Ip,j} v_{iI}^f + N_{Ip,i} v_{jI}^f) \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleStrainIncrementFluid(Mesh *mesh, vector<Body *> *bodies, double time_step);

	/// \brief Interpolate vorticity increment of solid at particle
	///
	/// \f$ \Delta \Omega_{ijp} = \frac{1}{2}(N_{Ip,j} v_{iI} - N_{Ip,i} v_{jI}) \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleVorticityIncrement(Mesh *mesh, vector<Body *> *bodies, double time_step);

	/// \brief Interpolate deformation gradient of solid at particle
	///
	/// \f$ F_{ijp}^{s,n+1} = (\delta_{ik} + \delta t \sum_I N_{Ip,k} v_{iI}^{s,n}) F_{kjp}^{s,n} \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleDeformationGradient(Mesh *mesh, vector<Body *> *bodies, double time_step);

	/// \brief Interpolate a vector3d in time using linear interpolation
	///	\param[in] times A list of times
	///	\param[in] values A list of vector3d values
	/// \param[out] interpolated_vector Interpolated vector3d
	Eigen::Vector3d interpolateVector(const std::vector<double>& times, const std::vector<Eigen::Vector3d>& values, double itime);
};

#endif /* INTERPOLATION_H_ */
