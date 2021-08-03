/*
 * Interpolation.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INTERPOLATION_H_
#define INTERPOLATION_H_

#include<vector>
using std::vector;

class Mesh;
class Body;

/// \namespace Interpolation
/// \brief Represents the interpolation operations in the MPM.
/// 
/// This class allows to interpolate quantities from particles to node and
/// from nodes to particle back.
namespace Interpolation {
	
	/// \brief Interpolate mass from particles to nodes
	///
	/// \f$ m_I^s = \sum_p m_p^s N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list of Bodies
	void nodalMass(Mesh* mesh, vector<Body*>* bodies);
	
	/// \brief Interpolate fluid mass from particles to nodes
	///
	/// \f$ m_I^f = \sum_p m_p^f N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list of Bodies
	void nodalMassFuid(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Interpolate momentum from particles to nodes
	///
	/// \f$ p_{iI}^s = \sum_p p_{ip}^s N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalMomentum(Mesh* mesh, vector<Body*>* bodies);
	
	/// \brief Interpolate fluid momentum from particles to nodes
	///
	/// \f$ p_{iI}^f = \sum_p p_{ip}^f N_{Ip}\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalMomentumFluid(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Interpolate internal force from particles to nodes
	/// 
	/// In one phase calculation:
	/// \f$ f_{iI}^{\text{int}}=-\sum_p \sigma_{ijp} V_p N_{Ip,i}\f$
	///
	/// In two phase calculation:
	/// \f$ f_{iI}^{\text{int},s}=-\sum_p \sigma_{ijp}' N_{Ip,i} V_p + \sum_p p^f N_{Ip,i} V_p\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalInternalForce(Mesh* mesh, vector<Body*>* bodies);
	
	/// \brief Interpolate internal force of fluid from particles to nodes
	///
	/// \f$ f_{iI}^{\text{int},f}= \sum_p n p^f N_{Ip,i} V_p\f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalInternalForceFluid(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Interpolate external force from particles to nodes
	///
	/// \f$ f_{iI}^{\text{ext,s}}=\sum_p b_{ip} m_p^f N_{Ip} + \sum_p b_{ip} m_p^s N_{Ip} \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalExternalForce(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Interpolate external force of fluid from particles to nodes
	///
	/// \f$ f_{iI}^{\text{ext,f}}=\sum_p b_{ip} m_p^w N_{Ip} - \sum_p \frac{m_p^f n g}{k_{ijp}}(v^w_{jp}-v^s_{jp})N_{Ip} \f$
	///
	/// At the moment, only the principal values of the hydraulic conductivity in 3D are considered, thus:
	///
	/// \f$ f_{iI}^{\text{ext,f}}=\sum_p b_{ip} m_p^w N_{Ip} - \sum_p \frac{m_p^f n g}{k_{ip}}(v^w_{ip}-v^s_{ip})N_{Ip} \f$
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalExternalForceFluid(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Interpolate the strain increment at particle
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleStrainIncrement(Mesh* mesh, vector<Body*>* bodies, double time_step);

	/// \brief Interpolate the strain increment of fluid at particle
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleStrainIncrementFluid(Mesh* mesh, vector<Body*>* bodies, double time_step);
	
	/// \brief Interpolate the vorticity increment at particle
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleVorticityIncrement(Mesh* mesh, vector<Body*>* bodies, double time_step);
};

#endif /* INTERPOLATION_H_ */