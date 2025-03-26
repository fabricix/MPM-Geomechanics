// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INC_DYNAMICRELAXATION_H_
#define INC_DYNAMICRELAXATION_H_

#include "Body/Body.h"

namespace DynamicRelaxation {

    double computeKineticEnergy(vector<Body *> *bodies);

    void setStaticSolution(vector<Body *> *bodies, int loopCounter);
}

#endif /* INC_DYNAMICRELAXATION_H_ */
