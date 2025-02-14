/*
 * Contact.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef CONTACT_H_
#define CONTACT_H_

#include "Mesh/Mesh.h"
#include "Body/Body.h"

/// \class Contact
/// \brief Represents a frictional contact between bodies
class Contact {

public:
	
	/// \brief Default constructor
	///
	Contact(); 
	
	/// \brief Default destructor
	///
	virtual ~Contact();

	/// \brief Check Contact
	/// \param[in] mesh
	/// \param[in] bodies
	static void checkContact(Mesh* mesh, vector<Body*>* bodies);

	/// \brief First contact check
	/// \param[in] mesh
	/// \param[in] bodies
	static void firstContactCheck(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Second contact check
	/// \param[in] mesh
	/// \param[in] bodies
	static void secondContactCheck(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Contact force
	/// \param[in] mesh
	/// \param[in] bodies
	/// \param[in] dt
	static void contactForce(Mesh* mesh, vector<Body*>* bodies, double dt);

	/// \brief Set Particles in Contact
	/// \param[in] mesh
	/// \param[in] bodies
	static void setParticlesInContact(Mesh* mesh, vector<Body*>* bodies);

	/// \brief Reset Particles in Contact
	/// \param[in] mesh
	/// \param[in] bodies
	static void resetParticlesInContact(vector<Body*>* bodies);
};

#endif /* CONTACT_H_ */
