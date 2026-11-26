// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team
#ifndef SHAPELINEARMULTILEVEL_H_
#define SHAPELINEARMULTILEVEL_H_

#include "Shape/Shape.h"
#include "Mesh/Cell.h"
#include <stdexcept>

/// Linear hexahedral interpolation with optional midpoint edge and face nodes.
/// Local indices 0..7 follow Cell's corner order; 8..25 follow HangingNodePosition.
class ShapeLinearMultiLevel : public Shape {
public:
    void setCellAndNode(const Cell& cell, std::size_t localNode);
    void update(const Vector3d& particlePosition, const Vector3d& nodePosition,
                const Vector3d& cellDimension, const Vector3d& particleSize) override;
    // Legacy separable components are undefined for transition functions.
    const Vector3d& getShape() const override { throw std::logic_error("Use getWeight for multilevel shapes"); }
    const Vector3d& getDerivate() const override { throw std::logic_error("Use getGradient for multilevel shapes"); }
    double getWeight() const override { return weight; }
    Vector3d getGradient() const override { return gradient; }
private:
    // The multidimensional transition functions are not separable into 1D factors.
    double computeGradient(double, double, double) override;
    double computeShape(double, double, double) override;
    const Cell* currentCell = nullptr;
    std::size_t currentNode = 0;
    double weight = 0.0;
    Vector3d gradient = Vector3d::Zero();
};
#endif
