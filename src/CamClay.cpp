// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2026 MPM-Geomechanics Development Team

#include <cmath>

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include <Materials/Material.h>
#include <Materials/CamClay.h>
#include "Warning.h"

CamClay::CamClay(int id, double density, double young, double poisson)
:ElasticJaumann(id, density, young, poisson) {

    // Model parameters
    // lamda, l, compresion index
    // kappa, k, sweelling index
    // Mc, slope of critical line (p-q) for compression
    // Me, slope of critical line (p-q) in extension 
    // n = Me/Mc
    // v = poisson ratio

    // configure material type
    type=Material::MaterialType::CAMCLAY;
}

CamClay::~CamClay() { }

void CamClay::updateStress(Particle *particle) const
{
    // update stress as elastic stress
    ElasticJaumann::updateStress(particle);
}
