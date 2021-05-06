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
	Interpolation();
	virtual ~Interpolation();
	void interpolateMassToNodes(Mesh&, vector<Particle>&);
	void interpolateMomentumToNodes(Mesh&, vector<Particle>&);
	void interpolateInternalForceToNodes(Mesh&, vector<Particle>&);
	void interpolateExternalForceToNodes(Mesh&, vector<Particle>&);
};

#endif /* INTERPOLATION_H_ */
