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
	
	// getters
    int getId() const { return id; }
    double getFrictionCoefficient() const { return frictionCoefficient; }
    int getMasterId() const { return masterId; }
    int getSlaveId() const { return slaveId; }
    const string& getNormalType() const { return normalType; }

    // setters
    void setId(int new_id) { id = new_id; }
    void setFrictionCoefficient(double mu) { frictionCoefficient = mu; }
    void setMasterId(int new_master_id) { masterId = new_master_id; }
    void setSlaveId(int new_slave_id) { slaveId = new_slave_id; }
    void setNormalType(const string& type) { normalType = type; }
	
	private:

	int id = 0;
	double frictionCoefficient = 0.0; //!< Friction coefficient \f$\mu\f$
	int masterId = 0; //<! Master body id
	int slaveId = 1; //<! Slave body id
	string normalType = "master"; //!< Normal type: "master", "slave" or "collinear"
};

#endif /* CONTACT_H_ */
