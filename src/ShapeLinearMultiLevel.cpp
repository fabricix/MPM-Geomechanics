// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team
#include "Shape/ShapeLinearMultiLevel.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace {
struct Basis {
    double value = 0.0;
    Vector3d gradient = Vector3d::Zero();
};
// Components -1 and +1 are cell ends; 0 denotes the midpoint hat function.
Basis product(const std::array<int,3>& kind, const Vector3d& local, const Vector3d& size) {
    double f[3], df[3];
    for (int k = 0; k < 3; ++k) {
        if (kind[k] == 0) {
            f[k] = 1.0 - std::abs(local[k]);
            // The midpoint hat has distinct one-sided derivatives at zero.
            // Use their average at the kink.
            df[k] = (local[k] > 0 ? -2.0 : (local[k] < 0 ? 2.0 : 0.0)) / size[k];
        } else {
            f[k] = 0.5 * (1.0 + kind[k] * local[k]);
            df[k] = kind[k] / size[k];
        }
    }
    Basis b;
    b.value = f[0]*f[1]*f[2];
    b.gradient = Vector3d(df[0]*f[1]*f[2], f[0]*df[1]*f[2], f[0]*f[1]*df[2]);
    return b;
}
void subtract(Basis& a, const Basis& b, double factor) {
    a.value -= factor*b.value;
    a.gradient -= factor*b.gradient;
}
bool incident(const std::array<int,3>& lower, const std::array<int,3>& higher) {
    for (int k=0;k<3;++k) if (higher[k] != 0 && lower[k] != higher[k]) return false;
    return true;
}
}

void ShapeLinearMultiLevel::setCellAndNode(const Cell& cell, std::size_t localNode) {
    if (localNode >= 26) throw std::out_of_range("Invalid local cell node");
    const auto corners = cell.getNodes();
    if (corners.size() != 8) throw std::invalid_argument("Expected eight corner nodes");
    for (auto* node : corners) if (!node) throw std::invalid_argument("Null corner node");
    if (localNode >= 8 && !cell.getHangingNode(static_cast<Cell::HangingNodePosition>(localNode-8)))
        throw std::invalid_argument("Selected hanging node is absent");
    currentCell = &cell;
    currentNode = localNode;
}

void ShapeLinearMultiLevel::update(const Vector3d& position, const Vector3d& nodePosition,
                                   const Vector3d& cellDimension, const Vector3d&) {
    if (!currentCell) throw std::logic_error("Cell and node context not set");
    const auto corners = currentCell->getNodes();
    const Vector3d min = corners[0]->getCoordinates();
    const Vector3d size(corners[1]->getCoordinates().x()-min.x(),
                        corners[2]->getCoordinates().y()-min.y(),
                        corners[4]->getCoordinates().z()-min.z());
    if ((size.array() <= 0).any() || !size.allFinite() || !position.allFinite())
        throw std::invalid_argument("Invalid cell dimensions or particle position");
    const Node* selected = currentNode < 8 ? corners[currentNode]
        : currentCell->getHangingNode(static_cast<Cell::HangingNodePosition>(currentNode-8));
    if (!selected || !nodePosition.isApprox(selected->getCoordinates(), 1e-10) || !cellDimension.isApprox(size, 1e-10))
        throw std::invalid_argument("Node position or cell dimensions do not match the selected context");
    Vector3d local = 2.0*(position-min).cwiseQuotient(size)-Vector3d::Ones();
    if ((local.array().abs() > 1.0 + 1e-10).any())
        throw std::out_of_range("Particle outside selected cell");
    for (int k=0;k<3;++k) local[k] = std::max(-1.0, std::min(1.0, local[k]));

    // Fixed positions ordered exactly as Cell::HangingNodePosition.
    const std::array<std::array<int,3>,18> h = {{
        {{0,-1,-1}},{{0,1,-1}},{{0,-1,1}},{{0,1,1}},
        {{-1,0,-1}},{{1,0,-1}},{{-1,0,1}},{{1,0,1}},
        {{-1,-1,0}},{{1,-1,0}},{{-1,1,0}},{{1,1,0}},
        {{-1,0,0}},{{1,0,0}},{{0,-1,0}},{{0,1,0}},{{0,0,-1}},{{0,0,1}}
    }};
    std::array<Basis,18> hanging;
    std::array<bool,18> present{};
    for (std::size_t i=0;i<18;++i) present[i]=currentCell->getHangingNode(static_cast<Cell::HangingNodePosition>(i)) != nullptr;
    // Face functions first, then edge functions corrected by adjacent faces.
    for (std::size_t i=12;i<18;++i) if (present[i]) hanging[i]=product(h[i],local,size);
    for (std::size_t i=0;i<12;++i) if (present[i]) {
        hanging[i]=product(h[i],local,size);
        for (std::size_t j=12;j<18;++j) if (present[j] && incident(h[i],h[j])) subtract(hanging[i],hanging[j],0.5);
    }
    Basis result;
    if (currentNode >= 8) result=hanging[currentNode-8];
    else {
        std::array<int,3> corner{{(currentNode&1)?1:-1,(currentNode&2)?1:-1,(currentNode&4)?1:-1}};
        result=product(corner,local,size);
        for (std::size_t i=0;i<12;++i) if (present[i] && incident(corner,h[i])) subtract(result,hanging[i],0.5);
        for (std::size_t i=12;i<18;++i) if (present[i] && incident(corner,h[i])) subtract(result,hanging[i],0.25);
    }
    weight=result.value;
    gradient=result.gradient;
}

double ShapeLinearMultiLevel::computeShape(double, double, double) { throw std::logic_error("Non-separable shape"); }
double ShapeLinearMultiLevel::computeGradient(double, double, double) { throw std::logic_error("Non-separable shape"); }
