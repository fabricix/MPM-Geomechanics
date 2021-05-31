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
class Particle;

/// \namespace Interpolation
/// \brief Represents the interpolation operations in the MPM.
/// 
/// This class allows to interpolate quantities from particles to node and
/// from nodes to particle back.
namespace Interpolation {
	
	/// \brief Interpolate mass from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] particles A list o Particle pointers
	void nodalMass(Mesh& mesh, vector<Particle*>* particles);
	
	/// \brief Interpolate momentum from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] particles A list o Particle pointers
	void nodalMomentum(Mesh& mesh, vector<Particle*>* particles);
	
	/// \brief Interpolate internal force from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] particles A list o Particle pointers
	void nodalInternalForce(Mesh& mesh, vector<Particle*>* particles);
	
	/// \brief Interpolate external force from particles to nodes
	/// \param[in] mesh Mesh reference
	/// \param[in] particles A list o Particle pointers
	void nodalExternalForce(Mesh& mesh, vector<Particle*>* particles);

	/// \brief Interpolate the strain increment at particle
	/// \param[in] mesh Mesh reference
	/// \param[in] particles A list o Particle pointers
	/// \param[in] time_step Time step
	void particleStrainIncrement(Mesh& mesh, vector<Particle*>* particles, double time_step);
	
	/// \brief Interpolate the vorticity increment at particle
	/// \param[in] mesh Mesh reference
	/// \param[in] particles A list o Particle pointers
	/// \param[in] time_step Time step
	void particleVorticityIncrement(Mesh& mesh, vector<Particle*>* particles, double time_step);
};

#endif /* INTERPOLATION_H_ */
