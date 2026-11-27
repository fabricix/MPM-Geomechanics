// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Excavation.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace Excavation {

	namespace {

		struct ExcavationEvent {
			double time;
			Body* body;
		};

		std::vector<ExcavationEvent> excavationEvents;
		size_t nextExcavation = 0;

	} // namespace

	void configure(const std::vector<Body*>& bodies) {
		excavationEvents.clear();
		nextExcavation = 0;

		for (Body* body : bodies) {
			if (body != nullptr && body->hasExcavationTime()) {
				excavationEvents.push_back({body->getExcavationTime(), body});
			}
		}

		std::stable_sort(excavationEvents.begin(), excavationEvents.end(),
			[](const ExcavationEvent& a, const ExcavationEvent& b) {
				return a.time < b.time;
			});
	}

	void apply(double stepTime) {
		while (nextExcavation < excavationEvents.size()) {
			const ExcavationEvent& event = excavationEvents[nextExcavation];
			const double tolerance = 8.0 * std::numeric_limits<double>::epsilon()
				* std::max(std::abs(stepTime), std::abs(event.time));

			if (stepTime < event.time && (event.time - stepTime) > tolerance) {
				break;
			}

			event.body->deactivateParticles();
			++nextExcavation;
		}
	}

	void clear() {
		excavationEvents.clear();
		nextExcavation = 0;
	}

	bool hasEvents() {
		return !excavationEvents.empty();
	}

	size_t pendingCount() {
		return (nextExcavation < excavationEvents.size())
			? (excavationEvents.size() - nextExcavation)
			: 0;
	}

} // namespace Excavation
