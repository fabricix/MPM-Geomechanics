/*
 * Utilities.h
 *
 *  Created on: 14 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INC_UTILITIES_H_
#define INC_UTILITIES_H_

namespace Utils{

template<typename T> T sgn(T val){
	return(val<0.0?-1.0:1.0);
}

}

#endif /* INC_UTILITIES_H_ */
