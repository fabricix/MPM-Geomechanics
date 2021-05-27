/*
 * Contact.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef CONTACT_H_
#define CONTACT_H_

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
};

#endif /* CONTACT_H_ */
