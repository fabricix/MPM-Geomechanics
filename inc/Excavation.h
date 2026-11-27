// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef EXCAVATION_H_
#define EXCAVATION_H_

#include <vector>
#include "Body/Body.h"

namespace Excavation {

	/// Configure excavation schedule from bodies carrying an excavation_time
	void configure(const std::vector<Body*>& bodies);

	/// Deactivate particles of bodies whose excavation_time <= stepTime
	void apply(double stepTime);

	/// Clear registered excavation events
	void clear();

	/// Return whether any excavation events are registered
	bool hasEvents();

	/// Return number of pending excavation events
	size_t pendingCount();

} // namespace Excavation

#endif /* EXCAVATION_H_ */
