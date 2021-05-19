/*
 * Update.h
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include "Mesh.h"
#include "Particle.h"

/// \namespace Update
/// \brief Represents operations to update values in nodes and particles.
namespace Update {

	void nodalVelocity(Mesh&); //!< updates the nodal velocity
	void nodalTotalForce(Mesh&); //!< updates the nodal total force
	void resetNodalValues(Mesh&); //!< deletes all stored values in nodes 
	void particleDensity(vector<Particle*>&); //!< updates the particles' density
	void particleStress(vector<Particle*>&); //!< update the particles' stress
	void particleVelocity(Mesh&, vector<Particle*>&,double); //!< update the particle velocity
	void particlePosition(Mesh&, vector<Particle*>&,double); //!< update the particle position
	void boundaryConditionsForce(Mesh& mesh); //!< update essential boundary condition
	void boundaryConditionsMomentum(Mesh& mesh); //!< update essential boundary condition
	void contributionNodes(Mesh&, vector<Particle*>&); //!< for each particle update the weights in each nodes that contributes

	void setPlaneForce(const Boundary::planeBoundary*, vector<Node>*, int dir); //!< the restriction in each node in boundary planes
	void setPlaneMomentum(const Boundary::planeBoundary*, vector<Node>*, int dir); //!< the restriction in each node in boundary planes
};

#endif /* UPDATE_H_ */
