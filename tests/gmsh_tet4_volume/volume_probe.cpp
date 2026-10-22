// SPDX-License-Identifier: MIT
// Observation harness: reads original Input API and calls original body creation.
// It does not implement particle seeding or advance the simulation.
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include "Input.h"
#include "Body/BodyGmsh.h"
#include "Model.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: volume_probe case.json exact_volume expected_particle_count\n";
        return 2;
    }
    try {
        const double exactVolume = std::stod(argv[2]);
        const size_t expectedCount = std::stoul(argv[3]);
        ModelSetup::setTwoPhaseActive(false);
        BodyGmsh::resetSharedMesh();
        Input::readInputFile(argv[1]);
        auto materials = Input::getMaterialList();
        auto bodies = Input::getBodyList(&materials);
        if (materials.size() != 1 || bodies.size() != 1)
            throw std::runtime_error("This reproduction requires one material and one body");
        auto* material = materials.at(0);
        auto* body = bodies.at(0);
        if (body->getMaterialId() != material->getId() || material->getPorosity() != 0.0)
            throw std::runtime_error("Unexpected material mapping or porosity");
        Mesh grid;
        grid.setCellDimension(Input::getCellDimension());
        body->create(grid, material);
        double mass = 0.0, volume = 0.0, initialVolume = 0.0;
        for (const auto* particle : *body->getParticles()) {
            mass += particle->getMass();
            volume += particle->getCurrentVolume();
            initialVolume += particle->getInitialVolume();
        }
        const double expectedMass = material->getDensity() * exactVolume;
        const auto close = [](double observed, double expected) {
            return std::isfinite(observed) &&
                std::abs(observed - expected) <= 1e-12 * std::abs(expected);
        };
        const size_t count = body->getParticles()->size();
        const bool passed = count == expectedCount && close(mass, expectedMass) && close(volume, exactVolume);
        std::cout << "MEASUREMENT " << std::setprecision(17)
                  << "{\"particles\":" << count
                  << ",\"expected_particles\":" << expectedCount
                  << ",\"mass\":" << mass
                  << ",\"expected_mass\":" << expectedMass
                  << ",\"current_volume\":" << volume
                  << ",\"initial_volume\":" << initialVolume
                  << ",\"expected_volume\":" << exactVolume
                  << ",\"mass_ratio\":" << mass / expectedMass
                  << ",\"mass_and_current_volume_ok\":" << (passed ? "true" : "false") << "}\n";
        for (auto* particle : *body->getParticles()) delete particle;
        delete body;
        delete material;
        BodyGmsh::resetSharedMesh();
        return passed ? 0 : 1;
    } catch (...) {
        std::cerr << "Could not construct the observation case\n";
        return 2;
    }
}
