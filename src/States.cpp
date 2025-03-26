// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "States.h"

#include <fstream>
#include <iostream>

#include "Eigen/Core"
using Eigen::Vector3d;
using Eigen::Matrix3d;

#include "Json/json.hpp"
using json = nlohmann::json;

void States::saveParticleStress(const std::string& filename, const std::vector<Particle*>& particles) {

    // initialize json structure    
    json jsonData;
    
    // for each particle append data to be saved in the json 
    for (size_t i = 0; i < particles.size(); ++i) {
        
        // get particle
        const auto* particle = particles[i];
        
        // init json data to be appended
        json particleData;

        // fill data structure with index (used for find the particle during loading states)
        particleData["index"] = i;

        // fill data structure with the stress of the particleS
        particleData["stress"] = {
            { particle->getStress()(0,0), particle->getStress()(0,1), particle->getStress()(0,2)},
            { particle->getStress()(1,0), particle->getStress()(1,1), particle->getStress()(1,2) },
            { particle->getStress()(2,0), particle->getStress()(2,1), particle->getStress()(2,2) }
        };

        // append filled data in to the exportable json structure
        jsonData.push_back(particleData);
    }

    // create output file and verify if is it open
    std::ofstream outFile("particle_stress_data.json");
    if (!outFile.is_open()) {
        std::cerr << "Error: can not open stress field file ..." << std::endl;
        return;
    }

    // write json data in to the file
    outFile << jsonData << std::endl;
    outFile.close();
    std::cout << "Stress field exported correctly ..." << std::endl;
}

void States::loadParticleStress(const std::string& filename, std::vector<Particle*>& particles) {

    // open state file and check if is it open
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error: Can not open particle stress file." << std::endl;
        return;
    }

    // initialize json structure
    json jsonData;

    // read the file
    inFile >> jsonData;
    
    // get each particle data
    for (const auto& particleData : jsonData) {
        
        // get particle by read index
        Particle* particle = particles.at(particleData["index"]);

        // get stress from data
        auto stress = particleData["stress"];

        // define a stress tensor and filled it with data
        Matrix3d sigma; 
        sigma.setZero();
        for (int i = 0; i < sigma.rows(); i++) { 
            for (int j = 0; j < sigma.cols(); j++) {    
                sigma(i, j) = stress[i][j];
            }     
        }
        // set stress in particle
        particle->setStress(sigma);
    }

    // close file and print information
    inFile.close();
    std::cout << "Stress field loaded ..." << std::endl;
}