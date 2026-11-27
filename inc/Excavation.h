// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef EXCAVATION_H_
#define EXCAVATION_H_

#include <vector>
#include "Body/Body.h"

namespace Excavation {

	/**
	 * @brief Configure the excavation schedule from the supplied bodies.
	 * @param bodies Bodies carrying an excavation_time used to register excavation events.
	 */
	void configure(const std::vector<Body*>& bodies);

	/**
	 * @brief Apply excavation events whose scheduled time has been reached.
	 * @param stepTime Current simulation step time.
	 */
	void apply(double stepTime);

	/**
	 * @brief Clear all registered excavation events.
	 *
	 * This resets the excavation schedule, leaving no pending or applied events.
	 */
	void clear();

	/**
	 * @brief Check whether any excavation events are registered.
	 * @return true if one or more excavation events are registered, false otherwise.
	 */
	bool hasEvents();

	/**
	 * @brief Return the number of pending excavation events.
	 * @return Number of excavation events that have not yet been applied.
	 */
	size_t pendingCount();

	/**
	 * @brief Check whether excavation events are scheduled.
	 * @return true if one or more excavation events are scheduled, false otherwise.
	 */
	bool hasScheduledExcavations();

} // namespace Excavation

#endif /* EXCAVATION_H_ */
