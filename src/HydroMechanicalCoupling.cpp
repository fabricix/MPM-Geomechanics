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

    void configureOneDirectionCoupling(std::vector<Particle*>& particles) {
        
        // Check if hydro-mechanical coupling is enabled and set the type
        if (Input::getHydroMechCouplingEnabled() && Input::getHydroMechCouplingOneWay())
        {
            ModelSetup::setHydroMechanicalCouplingType(ModelSetup::HydroMechanicalCouplingType::ONE_WAY);

            // Check if unsaturated analysis is enabled and setting the model setup
            if(Input::getUnsaturatedEnabled())
            {
                ModelSetup::setUnsaturatedAnalysisActive(true);
                BishopChi::setParameters(Input::getChiParameters());
            }
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

        // Variables for unsaturated analysis
        const bool unsat = ModelSetup::getUnsaturatedAnalysisActive();

        // Assign pressure to particles based on position
        for (const auto& entry : jpressure) {

            // Skip invalid entries or those missing required fields
            if (!entry.contains("position") || !entry.contains("pressure")) { continue; }
            
            Vector3d pos(entry["position"][0], entry["position"][1], entry["position"][2]);
            double pressure = entry["pressure"];

            // For unsaturated analysis, read saturation if available
            double Sr_val = 1.0; // default for saturated
            if (unsat && entry.contains("saturation") && entry["saturation"].is_number()) {
                Sr_val = std::clamp(entry["saturation"].get<double>(), 0.0, 1.0);
            }

            bool assigned = false;

            for (auto* particle : particles) {
                if ((particle->getPosition() - pos).norm() < tolerance) {
                    particle->setPorePressure(pressure);
                    assigned = true;

                    // For unsaturated analysis, set saturation
                    if (unsat) {
                        particle->setSaturation(Sr_val);
                    }
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
