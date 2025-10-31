// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Contact.h"

Contact::Contact(int id, double friction, int master_id, int slave_id, string normal_type):
	id(id > 0 ? id : 0),
	frictionCoefficient(friction),
	masterId(master_id > 0 ? master_id : 0),
	slaveId(slave_id > 0 ? slave_id : 1),
	normalType((normal_type != "collinear" && normal_type != "slave") ? "master" : normal_type)
{}

Contact::~Contact() { }