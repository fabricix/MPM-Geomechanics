/*
 * Warning.h
 *
 *  Created on: 21 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INC_WARNING_H_
#define INC_WARNING_H_

#include<string>
using std::string;

/// \namespace Warning
/// \brief This namespace contain functions to print 
/// coloured text indicating possible error or inconsistency in data.
namespace Warning
{	
	/// \brief Prints a warning message in terminal
	/// \param[in] message String containing the message to print
    void printMessage(string);
}

#endif /* INC_WARNING_H_ */
