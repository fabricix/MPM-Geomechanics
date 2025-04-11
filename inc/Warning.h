// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

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
    void printMessage(string message);
}

#endif /* INC_WARNING_H_ */
