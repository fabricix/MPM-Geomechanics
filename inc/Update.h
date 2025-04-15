// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef UPDATE_H_
#define UPDATE_H_

#include "Mesh/Mesh.h"
#include "Body/Body.h"
#include <Loads.h>

/// \namespace Update
/// \brief Represents operations to update values in nodes and particles.
namespace Update {

	/// \enum Direction
	/// \brief Principal direction in the model.
	/// This enumeration is used to update the boundary conditions in each direction
	enum Direction {X, Y, Z};

	/// \brief Update the nodal velocity
	///
	/// \f$ v_{i I}= p_{i I} / m_{I} \f$
	///
	/// \param[in] mesh Mesh reference
	void nodalVelocity(Mesh* mesh);
	
	/// \brief Update the nodal total force
	///
	/// In one phase calculations:
	///
	/// \f$ f_{iI}^{tot} = f_{iI}^{int} + f_{iI}^{ext} + f_{iI}^{dmp} \f$
	///
	/// In two phase calculations:
	///
	/// \f$ f_{iI}^{tot,s} = f_{iI}^{int,s} + f_{iI}^{ext,s} + f_{iI}^{dmp,s} - \dot{v}_{iI}^f m_I^f \f$
	///
	/// \param[in] mesh Mesh reference
	void nodalTotalForce(Mesh* mesh); 
	
	/// \brief Delete all stored values in nodes 
	/// \param[in] mesh Mesh reference
	void resetNodalValues(Mesh* mesh);
	
	/// \brief Updates the particles' density
	///
	/// In one phase calculations:
	///
	/// \f$ \rho_{p}^{t+1}=\rho_{p}^{t} /\left(1+\Delta \epsilon_{i i p}^{t-1/2}\right) \f$
	///
	/// In two phase calculation:
	/// 
	/// \f$ \rho_{p}^{t+1,s}=\rho_{p}^{t,s} /\left(1+\Delta \epsilon_{i i p}^{t-1/2,s}\right) \f$
	///
	/// \f$ \rho_{p}^{t+1,f}=\rho_{p}^{t,f} /\left(1+\Delta \epsilon_{i i p}^{t-1/2,f}\right) \f$
	///
	/// \param[in] bodies List of Body pointers
	void particleDensity(vector<Body*>* bodies);
	
	/// \brief Updates the porosity of the mixture
	///
	/// \f$ n^{t+1} = V^{t+1,f}/(V^{t+1,f}+V^{t+1,s})\f$
	///
	/// \param[in] bodies List of Body pointers
	void particlePorosity(vector<Body*>* bodies);

	/// \brief Update the particles' stress
	/// \param[in] bodies List of Body pointers
	void particleStress(vector<Body*>* bodies);
	
	/// \brief Update the particles pressure
	///
	/// \f$ p_p^{t+1,f} = p_p^{t,f} - \Delta t K_w / n^{t+1} ((1-n^{t+1}) v_{i,i}^{t+1/2,s}+ n^{t+1} v_{i,i}^{t+1/2,f} ) \f$
	///
	/// \param[in] bodies List of Body pointers
	/// \param[in] dt Time step
	void particlePressure(vector<Body*>* bodies, double dt);

	/// \brief Update the particle velocity
	///
	///	\f$ v_{ip}^{t+1/2} = v_{ip}^{t-1/2} + \sum_I N_{ip} f_{iI}^{t}/m_I \Delta t\f$ 
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of Body pointers
	/// \param[in] time_step Time step
	void particleVelocity(Mesh* mesh, vector<Body*>* bodies, double time_step);
	
	/// \brief Update the particle velocity fluid
	///
	/// \f$ v_{ip}^{f,t+1/2} = v_{ip}^{f,t-1/2} + \sum_I N_{ip} f_{iI}^{t,f}/m_I^f \Delta t \f$  
	///
	/// \param[in] mesh Mesh reference
	/// \param[in] bodies List of Body pointers
	/// \param[in] time_step Time step
	void particleVelocityFluid(Mesh* mesh, vector<Body*>* bodies, double time_step);

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
	/// terms of force of fluid phase
	/// \param[in] mesh Mesh reference
	void boundaryConditionsForceFluid(Mesh* mesh);

	/// \brief Apply essential boundary condition in
	/// terms of momentum
	/// \param[in] mesh Mesh reference
	void boundaryConditionsMomentum(Mesh* mesh);
	
	/// \brief Apply essential boundary condition in
	/// terms of momentum of fluid phase
	/// \param[in] mesh Mesh reference
	void boundaryConditionsMomentumFluid(Mesh* mesh);

	/// \brief Update the weights in each nodes that contributes
	/// \param[in] mesh Mesh reference
	/// \param[in] particles List of particles
	void contributionNodes(Mesh* mesh, vector<Particle*>* particles);

	/// \brief Configure the force in each node in boundary planes
	/// \param[in] boundary Boundary plane
	/// \param[in] nodes Node list pointer
	/// \param[in] direction Direction to apply de boundary condition 
	void setPlaneForce(const Boundary::planeBoundary* boundary, vector<Node*>* nodes, unsigned direction);
	
	/// \brief Configure the force in fluid phase in each node in boundary planes
	/// \param[in] boundary Boundary plane
	/// \param[in] nodes Node list pointer
	/// \param[in] direction Direction to apply de boundary condition 
	void setPlaneForceFluid(const Boundary::planeBoundary* boundary, vector<Node*>* nodes, unsigned direction);

	/// \brief Configure the momentum in each node in boundary planes
	/// \param[in] boundary Boundary plane
	/// \param[in] nodes Node list pointer
	/// \param[in] direction Direction to apply de boundary condition 
	/// \f$x=0\f$, \f$y=1\f$ , \f$z=2\f$ 
	void setPlaneMomentum(const Boundary::planeBoundary* boundary, vector<Node*>* nodes, unsigned direction);

	/// \brief Configure the momentum of fluid phase in each node in boundary planes
	/// \param[in] boundary Boundary plane
	/// \param[in] nodes Node list pointer
	/// \param[in] direction Direction to apply de boundary condition 
	/// \f$x=0\f$, \f$y=1\f$ , \f$z=2\f$ 
	void setPlaneMomentumFluid(const Boundary::planeBoundary* boundary, vector<Node*>* nodes, unsigned direction);
	
};

#endif /* UPDATE_H_ */
