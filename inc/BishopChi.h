// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#pragma once

namespace BishopChi 
{
    /// @brief Chi model for unsaturated analysis
    enum class Model { SR, SE, CONSTANT };
        
    /// @brief Configuration for chi model
    /// \details This struct holds the configuration parameters for the chi model used in unsaturated hydro
    struct Parameters  {
        Model model; // SR default
        double  sr_res; // used if model == SE
        double  chi_const; // used if model == CONSTANT
    };

    /// \brief Get chi model configuration
	/// \param[in] chi Chi model configuration
	void setParameters(BishopChi::Parameters chi);

	/// \brief Get chi model type
	BishopChi::Model getChiModelType();

    /// \brief Compute chi from saturation ratio Sr
    /// \param[in] Sr Saturation ratio
    /// \return chi Computed chi value
    double getChiFromSr(double Sr);
}