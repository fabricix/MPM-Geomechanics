/*
 * Node.h
 *
 *  Created on: 14 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef NODE_H_
#define NODE_H_

#include "Eigen/Core"
using Eigen::Vector3d;

/// \class Node
/// \brief Represents a mesh node
class Node {

public:
	
	/// \brief Default destructor
	///
	virtual ~Node();
	
	/// \brief Default constructor
	///
	Node();

	/// \brief Configure node identification
	/// \param[in] node_id Node identification
	inline void setId(int node_id) { this->id=node_id; }
	
	/// \brief Configure the active state on the node
	/// \param[in] node_activate Node active value
	inline void setActive(bool node_activate) { this->active=node_activate; }
	
	/// \brief Configure the nodal coordinates
	/// \param[in] nodal_coordinates Vector containing the nodal coordinates
	inline void setCoordinates(const Vector3d& nodal_coordinates) { this->coordinates=nodal_coordinates; }
	
	/// \brief Configure the nodal velocity
	/// \param[in] nodal_velocity Vector containing the nodal velocity
	inline void setVelocity(const Vector3d& nodal_velocity) { this->velocity=nodal_velocity; }
	
	/// \brief Configure the nodal momentum
	/// \param[in] nodal_momentum Vector containing the nodal momentum
	inline void setMomentum(const Vector3d& nodal_momentum) { this->momentum=nodal_momentum; }
	
	/// \brief Configure the nodal momentum of fluid phase
	/// \param[in] nodal_momentum_fluid Vector containing the nodal momentum of fluid phase
	virtual inline void setMomentumFluid(const Vector3d& nodal_momentum_fluid) { return; }

	/// \brief Configure the total nodal force
	/// \param[in] total_nodal_force Vector containing the total
	/// nodal force
	inline void setTotalForce(const Vector3d& total_nodal_force) { this->totalForce=total_nodal_force; }

	/// \brief Configure the total nodal force in fluid phase
	/// \param[in] total_nodal_force_fluid Vector containing the total
	/// nodal force in fluid phase
	virtual inline void setTotalForceFluid(const Vector3d& total_nodal_force_fluid) { return; }

	/// \brief Return the nodal identification
	/// \return Nodal identification 
	inline int getId() const { return this->id; }
	
	/// \brief Return the activate state of the node
	/// \return Nodal activate state
	inline bool getActive() const { return this->active; }
	
	/// \brief Return the nodal mass
	/// \return Nodal mass
	inline double getMass() const { return this->mass; }
	
	/// \brief Return the nodal mass of fluid phase
	/// \return Nodal mass of fluid phase
	virtual inline double getMassFluid() const { return 0.0; }

	/// \brief Return the nodal coordinates
	/// \return Vector containing the nodal
	/// coordinates
	inline const Vector3d& getCoordinates() const { return this->coordinates; }
	
	/// \brief Return the nodal momentum
	/// \return Vector containing the nodal momentum
	inline const Vector3d& getMomentum() const { return this->momentum; }
	
	/// \brief Return the nodal momentum of fluid phase
	/// \return Vector containing the nodal momentum of fluid phase
	virtual inline const Vector3d* getMomentumFluid() const { return NULL; }

	/// \brief Return the nodal internal force
	/// \return Vector containing the nodal internal force
	inline const Vector3d& getInternalForce() const { return this->internalForce; }
		
	/// \brief Return the nodal external force
	/// \return Vector containing the nodal external force
	inline const Vector3d& getExternalForce() const { return this->externalForce; }
		
	/// \brief Return the nodal total force
	/// \return Vector containing the nodal total force
	inline const Vector3d& getTotalForce() const { return this->totalForce; }
		
	/// \brief Return the nodal total force of fluid phase
	/// \return Vector containing the nodal total force of fluid phase
	virtual inline const Vector3d* getTotalForceFluid() const { return NULL; }

	/// \brief Return the nodal velocity
	/// \return Vector containing the nodal velocity
	inline const Vector3d& getVelocity() const { return this->velocity; }

	/// \brief Return the nodal velocity of fluid phase
	/// \return Vector containing the nodal velocity of fluid phase
	virtual inline const Vector3d* getVelocityFluid() const { return NULL; }

	/// \brief Add a mass increment to the nodal mass
	/// \param[in] mass_increment Nodal mass increment 
	inline void addMass(double mass_increment) { this->mass+=mass_increment; }

	/// \brief Add fluid mass increment to the nodal mass
	/// \param[in] fluid_mass_increment Nodal mass increment 
	virtual inline void addMassFluid(double fluid_mass_increment) { return; }

	/// \brief Add a momentum increment to the nodal momentum
	/// \param[in] momentum_increment Vector containing the nodal momentum increment 
	inline void addMomentum(const Vector3d& momentum_increment) { this->momentum+=momentum_increment; }

	/// \brief Add fluid momentum increment to the nodal momentum of fluid
	/// \param[in] fluid_momentum_increment Vector containing the nodal momentum increment of fluid
	virtual inline void addMomentumFluid(const Vector3d& fluid_momentum_increment) { return; }
		
	/// \brief Add a internal force increment to the nodal internal force
	/// \param[in] internal_force_increment Vector containing nodal internal force increment 
	inline void addInternalForce(const Vector3d& internal_force_increment) { this->internalForce+=internal_force_increment; }
	
	/// \brief Add a internal force increment of fluid to the nodal internal force
	/// \param[in] internal_force_fluid_increment Vector containing nodal internal force increment of fluid
	virtual inline void addInternalForceFluid(const Vector3d& internal_force_fluid_increment) { return; }
		
	/// \brief Add a external force increment to the nodal external force
	/// \param[in] external_force_increment Vector containing nodal external force increment 
	inline void addExternalForce(const Vector3d& external_force_increment) { this->externalForce+=external_force_increment; }

	/// \brief Add a external force of fluid increment to the nodal external force
	/// \param[in] external_force_fluid_increment Vector containing nodal external force increment 
	virtual inline void addExternalForceFluid(const Vector3d& external_force_fluid_increment) { return; }
	
	/// \brief Delete all values stored in node
	///
	virtual void resetValues();

	/// \brief Calculate the total nodal force
	///
	virtual inline void updateTotalForce() { this->totalForce = this->internalForce + this->externalForce + this->dampingForce; }

	/// \brief Calculate the damping nodal force
	///
	virtual void updateDampingForce();

	/// \brief Update nodal velocity
	///
	inline void updateVelocity(){ this->velocity = this->momentum / this->mass; }

	/// brief Integrate momentum
	///
	virtual inline void integrateMomentum(double dt) { this->momentum +=  this->totalForce*dt; }

protected:

	bool active; //!< is active node

	int id; //!< nodal identification

	double mass; //!< nodal mass: \f$m_{I}\f$, or solid mass in two-phase calculations: \f$m_{I}^{s}\f$

	Vector3d coordinates; //!< nodal coordinates: \f$x_{iI}\f$
	Vector3d momentum; //!< nodal momentum: \f$p_{iI}\f$, or momentum in solid in two-phase calculations: \f$p_{iI}^{s}\f$
	Vector3d velocity; //!< nodal velocity: \f$v_{iI}\f$, or velocity in solid in two-phase calculations: \f$v_{iI}^{s}\f$
	Vector3d externalForce; //!< nodal external force: \f$f_{iI}^{ext}\f$, or external force in solid in two-phase calculations: \f$f_{iI}^{ext,s}\f$
	Vector3d internalForce; //!< nodal internal force: \f$f_{iI}^{int}\f$, or internal force in solid in two-phase calculations: \f$f_{iI}^{int,s}\f$
	Vector3d dampingForce; //!< nodal damping force: \f$f_{iI}^{dmp}\f$, or damping force in solid in two-phase calculations: \f$f_{iI}^{dmp,s}\f$
	Vector3d totalForce; //!< nodal total force: \f$f_{iI}\f$, or total force in solid in two-phase calculations: \f$f_{iI}^{s}\f$
	
	double distanceLS; //!< distance level set function value of the node: \f$ d_{I}=(X_I-X_i)e_n \f$
};

inline Node::Node() {

    active=false;
    id=0;
    mass=0.0;
    coordinates.setZero();
    momentum.setZero();
    velocity.setZero();
    externalForce.setZero();
    internalForce.setZero();
    totalForce.setZero();
    dampingForce.setZero();
}

inline void Node::resetValues()
{
    active=false;
    mass=0.0;
    momentum.setZero();
    externalForce.setZero();
    internalForce.setZero();
}

inline Node::~Node() { }

#endif /* NODE_H_ */
