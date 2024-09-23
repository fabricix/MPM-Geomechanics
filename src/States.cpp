
#include "States.h"

#include "Json/json.hpp"

#include <fstream>

#include <iostream>

#include "Eigen/Core"

using Eigen::Vector3d;

using Eigen::Matrix3d;

using json = nlohmann::json;

void States::saveParticleStress(const std::string& filename, const std::vector<Particle*>& particles) {
    
    json jsonData;

    for (size_t i = 0; i < particles.size(); ++i) {
        
        const auto* particle = particles[i];
        
        json particleData;

        particleData["index"] = i;

        particleData["stress"] = {
            { particle->getStress()(0,0), particle->getStress()(0,1), particle->getStress()(0,2)},
            { particle->getStress()(1,0), particle->getStress()(1,1), particle->getStress()(1,2) },
            { particle->getStress()(2,0), particle->getStress()(2,1), particle->getStress()(2,2) }
        };

        jsonData.push_back(particleData);
    }

    std::ofstream outFile("particle_stress_data.json");

    if (!outFile.is_open()) {
        std::cerr << "Error: can not open stress field file ..." << std::endl;
        return;
    }

    outFile << jsonData << std::endl;
    outFile.close();
    std::cout << "Stress field exported correctly ..." << std::endl;
}

void States::loadParticleStress(const std::string& filename, std::vector<Particle*>& particles) {

    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error: Can not open particle stress file." << std::endl;
        return;
    }

    json jsonData;

    inFile >> jsonData;

    for (const auto& particleData : jsonData) {

        Particle* particle = particles.at(particleData["index"]);

        auto stress = particleData["stress"];

        Matrix3d sigma;

        sigma.setZero();
        for (int i = 0; i < sigma.rows(); i++) { 
        
            for (int j = 0; j < sigma.cols(); j++) {
            
                sigma(i, j) = stress[i][j];
            }     
        }
        particle->setStress(sigma);
    }

    inFile.close();

    std::cout << "Stress field loaded ..." << std::endl;
}