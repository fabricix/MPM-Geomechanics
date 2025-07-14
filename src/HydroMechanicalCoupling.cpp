// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "HydroMechanicalCoupling.h"
#include "Input.h"
#include "Model.h"
#include "Particle/Particle.h"

#include "Json/json.hpp"
using json = nlohmann::json;

#include <fstream>
#include <iostream>

namespace HydroMechanicalCoupling {

    bool isOneWayEnabled() {
        return ModelSetup::getHydroMechanicalCouplingType() == ModelSetup::HydroMechanicalCouplingType::ONE_WAY;
    }

    void configure(std::vector<Particle*>& particles) {
        
        // Check if hydro-mechanical coupling is enabled and set the type
        if (Input::getHydroMechCouplingEnabled() && Input::getHydroMechCouplingOneWay())
        {
            ModelSetup::setHydroMechanicalCouplingType(ModelSetup::HydroMechanicalCouplingType::ONE_WAY);
        }
        else
        {
            ModelSetup::setHydroMechanicalCouplingType(ModelSetup::HydroMechanicalCouplingType::NONE);
            return; // No hydro-mechanical coupling to configure
        }

        // Compute spatial tolerance based on mesh cell dimensions
        double tolerance = Input::getCellDimension().mean() * 0.5;

        // Read JSON file containing pore pressure field
        std::ifstream file(Input::getPorePressureFilename());
        if (!file.is_open()) {
            std::cerr << "[Error] Cannot open pore pressure file: "
                    << Input::getPorePressureFilename() << std::endl;
            return;
        }

        nlohmann::json jpressure;
        file >> jpressure;

        // Assign pressure to particles based on position
        for (const auto& entry : jpressure) {
            Vector3d pos(entry["position"][0], entry["position"][1], entry["position"][2]);
            double pressure = entry["pressure"];
            bool assigned = false;

            for (auto* particle : particles) {
                if ((particle->getPosition() - pos).norm() < tolerance) {
                    particle->setPorePressure(pressure);
                    assigned = true;
                    break;
                }
            }

            if (!assigned) {
                std::cerr << "[Warning] No particle found near position ("
                        << pos.transpose() << ") to assign pressure " << pressure << std::endl;
            }
        }
    }

    Eigen::Matrix3d computeTotalStress(const Eigen::Matrix3d& sigma_effective, double pore_pressure) {
        // Return total stress = effective stress + pore pressure * identity
        return sigma_effective + pore_pressure * Eigen::Matrix3d::Identity();
    }

} // namespace HydroMechanicalCoupling
