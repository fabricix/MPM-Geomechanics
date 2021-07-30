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
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list of Bodies
	void nodalMass(Mesh* mesh, vector<Body*>* bodies);
	
	/// \brief Interpolate fluid mass from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list of Bodies
	void nodalMassFuid(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Interpolate momentum from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalMomentum(Mesh* mesh, vector<Body*>* bodies);
	
	/// \brief Interpolate internal force from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalInternalForce(Mesh* mesh, vector<Body*>* bodies);
	
	/// \brief Interpolate external force from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	void nodalExternalForce(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Interpolate the strain increment at particle
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleStrainIncrement(Mesh* mesh, vector<Body*>* bodies, double time_step);
	
	/// \brief Interpolate the vorticity increment at particle
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies A list o Body pointers
	/// \param[in] time_step Time step
	void particleVorticityIncrement(Mesh* mesh, vector<Body*>* bodies, double time_step);
};

#endif /* INTERPOLATION_H_ */
