// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Seismic.h"
#include "Input.h"

namespace Seismic
{
    // Seismic file information
    SeismicAnalysis seismic_analysis;
    
    // Seismic data record
    SeismicData seismicRecord;

    // Get seismic analysis information
    SeismicAnalysis& getSeismicAnalysis() { return seismic_analysis; }

    // Set seismic analysis information
    void setSeismicAnalysis(const SeismicAnalysis& info) 
    { 
        seismic_analysis = info; 
    }

    // Get seismic data
    SeismicData& getSeismicData() { return seismicRecord; }

    // Set seismic data from file
    void setSeismicData()
    {
	    if(!seismic_analysis.isActive) return;
	    seismicRecord = Input::readSeismicData(seismic_analysis.filename, seismic_analysis.hasHeader);
    }
}