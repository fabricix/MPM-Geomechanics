// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CONTACT_H_
#define CONTACT_H_
#include <string>
using namespace std;

/// \class Contact
/// \brief Represents a frictional contact between bodies
class Contact {

public:
	
	/// @brief  Constructor
	/// @param contact id
	/// @param friction Friction coefficient
	/// @param master_id ID of the master body
	/// @param slave_id ID of the slave body
	/// @param normal_type Normal type: "master", "slave" or "collinear"
	Contact(int id, double friction, int master_id, int slave_id, string normal_type);
	
	/// \brief Default destructor
	///
	virtual ~Contact();


	int id = 0;
	double frictionCoefficient = 0.0; //!< Friction coefficient \f$\mu\f$
	int masterId = 0; //<! Master body id
	int slaveId = 1; //<! Slave body id
	string normalType = ""; //!< Normal type: "master", "slave" or "collinear"
private:
};

#endif /* CONTACT_H_ */
