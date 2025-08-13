// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CONFIG_PARALLEL_H
#define CONFIG_PARALLEL_H

// Master switch: enable all validated parallel features
#ifdef USE_PARALLEL_COMPUTATION
	#define USE_PARALLEL_MASS
	#define USE_PARALLEL_MOMENTUM
	#define USE_PARALLEL_INTERNAL_FORCE
    #define USE_PARALLEL_EXTERNAL_FORCE
    #endif
#endif // CONFIG_PARALLEL_H
