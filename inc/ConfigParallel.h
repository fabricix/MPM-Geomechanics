// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef CONFIG_PARALLEL_H
#define CONFIG_PARALLEL_H

// Master switch: enable all validated parallel features
#ifdef USE_PARALLEL_COMPUTATION
	#define USE_PARALLEL_MASS
	#define USE_PARALLEL_MOMENTUM
    #define USE_PARALLEL_CONTRIBUTION
	#define USE_PARALLEL_INTERNAL_FORCE
    #define USE_PARALLEL_EXTERNAL_FORCE
    #define USE_PARALLEL_STRAIN_INCREMENT
    #define USE_PARALLEL_VORTICITY_INCREMENT
    #define USE_PARALLEL_DENSITY
    // #define USE_PARALLEL_FORCE
    // #define USE_PARALLEL_CONTACT
    #endif
#endif // CONFIG_PARALLEL_H
