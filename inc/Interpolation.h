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

/// \class Interpolation
/// \brief Represents the interpolation operations in the MPM.
/// 
/// This class allows to interpolate quantities from particles to node and
/// from nodes to particle back.
class Interpolation {

public:
	
	Interpolation(); //!< default constructor
	virtual ~Interpolation(); //!< default destructor
	
	void nodalMass(Mesh&, vector<Particle>&); //!< interpolate mass from particles to nodes
	void nodalMomentum(Mesh&, vector<Particle>&); //!< interpolate momentum from particles to nodes
	void nodalInternalForce(Mesh&, vector<Particle>&); //!< interpolate internal force from particles to nodes
	void nodalExternalForce(Mesh&, vector<Particle>&); //!< interpolate external force from particles to nodes

	void particleStrainIncrement(Mesh&, vector<Particle>&, double dt); //!< interpolate the strain increment at particle
	void particleVorticityIncrement(Mesh&, vector<Particle>&,double dt); //!< interpolate the vorticity increment at particle
};

#endif /* INTERPOLATION_H_ */
