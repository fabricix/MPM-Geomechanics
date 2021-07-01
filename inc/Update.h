/*
 * Update.h
 *
 *  Created on: 10 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include "Mesh/Mesh.h"
#include "Body/Body.h"

/// \namespace Update
/// \brief Represents operations to update values in nodes and particles.
namespace Update {

	/// \brief Update the nodal velocity
	/// \param[in] mesh Mesh reference
	void nodalVelocity(Mesh* mesh);
	
	/// \brief Update the nodal total force
	/// \param[in] mesh Mesh reference
	void nodalTotalForce(Mesh* mesh); 
	
	/// \brief Delete all stored values in nodes 
	/// \param[in] mesh Mesh reference
	void resetNodalValues(Mesh* mesh);
	
	/// \brief Updates the particles' density
	/// \param[in] bodies List of Body pointers
	void particleDensity(vector<Body*>* bodies);
	
	/// \brief Update the particles' stress
	/// \param[in] bodies List of Body pointers
	void particleStress(vector<Body*>* bodies);
	
	/// \brief Update the particle velocity
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of Body pointers
	/// \param[in] time_step Time step
	void particleVelocity(Mesh* mesh, vector<Body*>* bodies, double time_step);
	
	/// \brief Update particle position
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of Body pointers
	/// \param[in] time_step Time step
	void particlePosition(Mesh* mesh, vector<Body*>* bodies, double time_step);
	
	/// \brief Apply essential boundary condition in 
	/// terms of force
	/// \param[in] mesh Mesh reference
	void boundaryConditionsForce(Mesh* mesh);
	
	/// \brief Apply essential boundary condition in
	/// terms of momentum
	/// \param[in] mesh Mesh reference
	void boundaryConditionsMomentum(Mesh* mesh);
	
	/// \brief Update the weights in each nodes that contributes
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of bodies
	void contributionNodes(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Configure the force in each node in boundary planes
	/// \param[in] boundary Boundary plane
	/// \param[in] nodes Node list pointer
	/// \param[in] direction Direction to apply de boundary condition 
	/// \f$x=0\f$, \f$y=1\f$ , \f$z=2\f$ 
	void setPlaneForce(const Boundary::planeBoundary* boundary, vector<Node>* nodes, unsigned direction);
	
	/// \brief Configure the momentum in each node in boundary planes
	/// \param[in] boundary Boundary plane
	/// \param[in] nodes Node list pointer
	/// \param[in] direction Direction to apply de boundary condition 
	/// \f$x=0\f$, \f$y=1\f$ , \f$z=2\f$ 
	void setPlaneMomentum(const Boundary::planeBoundary* boundary, vector<Node>* nodes, unsigned direction);
};

#endif /* UPDATE_H_ */
