// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INC_PLASTICWORK_H_
#define INC_PLASTICWORK_H_

#include "Particle/Particle.h"

namespace PlasticWork
{
    const double getPlasticWork(); //!< Get current plastic work
    void setPlasticWork(double wp); //!< Set current plastic work
    void computePlasticWork(vector <Particle*>* particles); //!< Compute plastic work

} // namespace PlasticWork

#endif /* INC_PLASTICWORK_H_ */