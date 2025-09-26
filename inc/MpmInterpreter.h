// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef MPM_INTERPRETER_H_
#define MPM_INTERPRETER_H_

#include <string>

namespace MpmInterpreter
{
  // Interpret the .mpm file and return the path to the generated JSON file
  std::string interpreter(const std::string& filename);
}

#endif /* MPM_INTERPRETER_H_ */
