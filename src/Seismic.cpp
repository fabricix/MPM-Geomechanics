// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Seismic.h"
#include "Input.h"
#include "Interpolation.h"

#include <unordered_set>

namespace Seismic
{
    std::vector<int> seismicNodeIndices;
    std::unordered_set<int> seismicNodeSet;
    Eigen::Vector3d accumulatedVelocity = Eigen::Vector3d::Zero();

    // Seismic file information
    SeismicAnalysis seismic_analysis;
    
    // Seismic data record
    SeismicData seismicRecord;

    // Get seismic analysis information
    SeismicAnalysis& getSeismicAnalysis() { return seismic_analysis; }

    // Get accumulated velocity
    Eigen::Vector3d& getAccumulatedVelocity() { return accumulatedVelocity; }

    // Set seismic analysis information
    void setSeismicAnalysis(const SeismicAnalysis& info) { seismic_analysis = info; }

    // disable seismic analysis
    void disableSeismicAnalysis() 
    {
        ModelSetup::setSeismicAnalysisActive(false);
        seismic_analysis.isActive = false;
        seismicRecord.time.clear();
        seismicRecord.acceleration.clear();
        accumulatedVelocity = Eigen::Vector3d::Zero();
        seismicNodeIndices.clear();
        seismicNodeSet.clear();
    }

    // Get seismic data
    SeismicData& getSeismicData() { return seismicRecord; }

    // Set seismic data from file
    void setSeismicData()
    {
	    if(!seismic_analysis.isActive) return;
	    seismicRecord = Input::readSeismicData(seismic_analysis.filename, seismic_analysis.hasHeader);
    }

    // mark seismic nodes based on distance from level set
    void markSeismicNodes(double epsilon, Mesh* mesh)
    {
        if (!mesh) return;

        // Clear previous seismic node indices and set
        seismicNodeIndices.clear();
        seismicNodeSet.clear();

        // Iterate through nodes and mark those within epsilon distance from the level set    
        std::vector<Node*>* nodes = mesh->getNodes();
        seismicNodeIndices.clear();
        seismicNodeSet.clear();

        for (int i = 0; i < static_cast<int>(nodes->size()); ++i) {
            Node* node = nodes->at(i);
            double d = std::abs(node->getDistanceLevelSet());

            if (d < epsilon) {
                seismicNodeIndices.push_back(i);
                seismicNodeSet.insert(i);  // store in set for quick access
            }
        }
    }

    // get seismic node indices
    const std::vector<int>& getSeismicNodeIndices() { return seismicNodeIndices; }

    // check if a node is a seismic node
    bool isSeismicNode(int nodeId) { return seismicNodeSet.count(nodeId) > 0; }

    void updateAccumulatedSeismicVelocity(const double currentTime, const double dt) {

        // Interpolate seismic acceleration at the current time
        Eigen::Vector3d a_sismo = Interpolation::interpolateVector(
            seismicRecord.time,
            seismicRecord.acceleration,
            currentTime
        );

        // Integrate to get the accumulated velocity
        accumulatedVelocity += a_sismo * dt;
    }

    void applySeismicVelocityMarkedSTLNodes(double currentTime, double dt, Mesh* mesh)
    {
        std::vector<Node*>* nodes = mesh->getNodes();

        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(seismicNodeIndices.size()); ++i) {
            
            int nodeId = seismicNodeIndices[i];
            Node* node = nodes->at(nodeId);
            
            if (node->getActive()){
                node->setVelocity(accumulatedVelocity);
            }
        }
    }
}