// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Body/BodyGmsh.h"
#include "Particle/Particle.h"
#include "Model.h"

BodyGmsh::BodyGmsh():Body(){

    this->materialId = -1;
}

BodyGmsh::~BodyGmsh() { }

void BodyGmsh::create(Mesh& mesh, Material* material) {

    // seeds MPs in the body defined by a Gmsh mesh file
}
