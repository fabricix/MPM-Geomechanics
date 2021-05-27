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
/// \brief Represents a mesh node.
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
	inline void setCoordinates(Vector3d nodal_coordinates) { this->coordinates=nodal_coordinates; }
	
	/// \brief Configure the nodal velocity
	/// \param[in] nodal_velocity Vector containing the nodal velocity
	inline void setVelocity(Vector3d nodal_velocity) { this->velocity=nodal_velocity; }
	
	/// \brief Configure the nodal momentum
	/// \param[in] nodal_momentum Vector containing the nodal momentum
	inline void setMomentum(Vector3d nodal_momentum) { this->momentum=nodal_momentum; }
	
	/// \brief Configure the total nodal force
	/// \param[in] total_nodal_force Vector containing the total
	/// nodal force
	inline void setTotalForce(Vector3d total_nodal_force) { this->totalForce=total_nodal_force; }

	/// \brief Return the nodal identification
	/// \param[out] node_id Nodal identification 
	inline int getId() const { return this->id; }
	
	/// \brief Return the activate state of the node
	/// \param[out] is_node_active Nodal activate state
	inline bool getActive() const { return this->active; }
	
	/// \brief Return the nodal mass
	/// \param[out] nodal_mass Nodal mass
	inline double getMass() const { return this->mass; }
	
	/// \brief Return the nodal coordinates
	/// \param[out] nodal_coordinates Vector containing the nodal
	/// coordinates
	inline Vector3d getCoordinates() const { return this->coordinates; }
	
	/// \brief Return the nodal momentum
	/// \param[out] nodal_momentum Vector containing the nodal momentum
	Vector3d getMomentum() const { return this->momentum; }
		
	/// \brief Return the nodal internal force
	/// \param[out] nodal_internal_force Vector
	/// containing the nodal internal force
	inline Vector3d getInternalForce() const { return this->internalForce; }
		
	/// \brief Return the nodal external force
	/// \param[out] nodal_external_force Vector
	/// containing the nodal external force
	inline Vector3d getExternalForce() const { return this->externalForce; }
		
	/// \brief Return the nodal total force
	/// \param[out] nodal_total_force Vector
	/// containing the nodal total force
	inline Vector3d getTotalForce() const { return this->totalForce; }
		
	/// \brief Return the nodal velocity
	/// \param[out] nodal_velocity Vector
	/// containing the nodal velocity
	inline Vector3d getVelocity() const { return this->velocity; }

	/// \brief Add a mass increment to the nodal mass
	/// \param[in] mass_increment Nodal mass increment 
	inline void addMass(double mass_increment) { this->mass+=mass_increment; }
		
	/// \brief Add a momentum increment to the nodal momentum
	/// \param[in] momentum_increment Vector containing the nodal momentum increment 
	inline void addMomentum(Vector3d momentum_increment) { this->momentum+=momentum_increment; }
		
	/// \brief Add a internal force increment to the nodal internal force
	/// \param[in] internal_force_increment Vector containing nodal internal force increment 
	inline void addInternalForce(Vector3d internal_force_increment) { this->internalForce+=internal_force_increment; }
		
	/// \brief Add a external force increment to the nodal external force
	/// \param[in] external_force_increment Vector containing nodal external force increment 
	inline void addExternalForce(Vector3d external_force_increment) { this->externalForce+=external_force_increment; }
	
	/// \brief delete all values stored in node
	///
	void resetValues();

private:

	bool active; //!< is active node

	int id; //!< nodal identification

	double mass; //!< nodal mass
	
	Vector3d coordinates; //!< node coordinates

	Vector3d momentum; //!< nodal momentum

	Vector3d velocity; //!< nodal velocity

	Vector3d acceleration; //!< nodal acceleration

	Vector3d externalForce; //!<  total external force

	Vector3d internalForce; //!< nodal internal force
	
	Vector3d totalForce; //!< nodal total force
};

#endif /* NODE_H_ */
