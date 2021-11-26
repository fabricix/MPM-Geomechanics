/*
 * DynamicRelaxation.h
 *
 *  Created on: 4 de set de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INC_DYNAMICRELAXATION_H_
#define INC_DYNAMICRELAXATION_H_

#include "Body/Body.h"

namespace DynamicRelaxation {

    void setStaticSolution(vector<Body*>* bodies, int loopCounter);
}

#endif /* INC_DYNAMICRELAXATION_H_ */
