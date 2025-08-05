// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INC_ENERGY_H_
#define INC_ENERGY_H_

#include "Body/Body.h"

namespace Energy
{
    double getCurrentKineticEnergy(); //!< Get current kinetic energy
    void setCurrentKineticEnergy(double energy);  //<!< Set current kinetic energy
    double getLastKineticEnergy();  //!< Get last kinetic energy
    void setLastKineticEnergy(double energy);  //<!< Set last kinetic energy
    double deltaKineticEnergy(); //<!< Set last kinetic energy as current
    void computeKineticEnergy(vector<Particle*>* particles); //!< Compute the kinetic energy of the bodies
};

#endif /* INC_ENERGY_H_ */