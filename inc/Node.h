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
	void setId(int); 
	
	/// \brief Configure the active state on the node
	/// \param[in] node_activate Node active value
	void setActive(bool);
	
	/// \brief Configure the nodal coordinates
	/// \param[in] nodal_coordinates Vector containing the nodal coordinates
	void setCoordinates(Vector3d);
	
	/// \brief Configure the nodal velocity
	/// \param[in] nodal_velocity Vector containing the nodal velocity
	void setVelocity(Vector3d);
	
	/// \brief Configure the nodal momentum
	/// \param[in] nodal_momentum Vector containing the nodal momentum
	void setMomentum(Vector3d);
	
	/// \brief Configure the total nodal force
	/// \param[in] total_nodal_force Vector containing the total
	/// nodal force
	void setTotalForce(Vector3d);

	/// \brief Return the nodal identification
	/// \param[out] node_id Nodal identification 
	int getId() const;
	
	/// \brief Return the activate state of the node
	/// \param[out] is_node_active Nodal activate state
	bool getActive() const;
	
	/// \brief Return the nodal mass
	/// \param[out] nodal_mass Nodal mass
	double getMass() const;
	
	/// \brief Return the nodal coordinates
	/// \param[out] nodal_coordinates Vector containing the nodal
	/// coordinates
	Vector3d getCoordinates() const;
	
	/// \brief Return the nodal momentum
	/// \param[out] nodal_momentum Vector containing the nodal momentum
	Vector3d getMomentum() const;
		
	/// \brief Return the nodal internal force
	/// \param[out] nodal_internal_force Vector
	/// containing the nodal internal force
	Vector3d getInternalForce() const; 
		
	/// \brief Return the nodal external force
	/// \param[out] nodal_external_force Vector
	/// containing the nodal external force
	Vector3d getExternalForce() const; //!< returns the external force
		
	/// \brief Return the nodal total force
	/// \param[out] nodal_total_force Vector
	/// containing the nodal total force
	Vector3d getTotalForce() const;
		
	/// \brief Return the nodal velocity
	/// \param[out] nodal_velocity Vector
	/// containing the nodal velocity
	Vector3d getVelocity() const;

	/// \brief Add a mass increment to the nodal mass
	/// \param[in] mass_increment Nodal mass increment 
	void addMass(double);
		
	/// \brief Add a momentum increment to the nodal momentum
	/// \param[in] momentum_increment Vector containing the nodal momentum increment 
	void addMomentum(Vector3d);
		
	/// \brief Add a internal force increment to the nodal internal force
	/// \param[in] internal_force_increment Vector containing nodal internal force increment 
	void addInternalForce(Vector3d);
		
	/// \brief Add a external force increment to the nodal external force
	/// \param[in] external_force_increment Vector containing nodal external force increment 
	void addExternalForce(Vector3d);
	
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
