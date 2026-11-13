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
	/// @brief Contact normal selection and corresponding input strings.
	enum NormalType {
		MASTER,   //!< "master"
		COLLINEAR, //!< "collinear"
		SLAVE     //!< "slave"
	};
	
	/// @brief  Constructor
	/// @param contact id
	/// @param friction Friction coefficient
	/// @param master_id ID of the master body
	/// @param slave_id ID of the slave body
	/// @param normal_type Normal type: "master", "slave" or "collinear"

	Contact(int id, double friction, int master_id, int slave_id, NormalType normal_type);
	
	/// \brief Default destructor
	///
	virtual ~Contact();
	
	// getters
    int getId() const { return id; }
    double getFrictionCoefficient() const { return frictionCoefficient; }
    int getMasterId() const { return masterId; }
    int getSlaveId() const { return slaveId; }
    NormalType getNormalType() const { return normalType; }

    // setters
    void setId(int new_id) { id = new_id; }
    void setFrictionCoefficient(double mu) { frictionCoefficient = mu; }
    void setMasterId(int new_master_id) { masterId = new_master_id; }
    void setSlaveId(int new_slave_id) { slaveId = new_slave_id; }
    void setNormalType(NormalType type) { normalType = type; }
	static NormalType getNormalTypeString(const string& type) {
		if (type == "collinear") return COLLINEAR;
		if (type == "slave") return SLAVE;
		return MASTER;
	}
	
	private:

	int id = 0;
	double frictionCoefficient = 0.0; //!< Friction coefficient \f$\mu\f$
	int masterId = 0; //<! Master body id
	int slaveId = 1; //<! Slave body id
	NormalType normalType = MASTER; //!< Normal type: MASTER, COLLINEAR or SLAVE
};

#endif /* CONTACT_H_ */
