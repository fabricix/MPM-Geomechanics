// SPDX-License-Identifier: MIT
// Copyright (c) 2021-202 MPM-Geomechanics Development Team

#ifndef INC_MATERIALS_CAMCLAY_H_
#define INC_MATERIALS_CAMCLAY_H_

#include "Materials/Material.h"
#include "Particle/Particle.h"

#include <cstddef>

/// \class CamClay
/// \brief Represents an elasto-plastic material using the CamClay failure criteria 
class CamClay: public Material {
public:

    /// \brief Create a CamClay elasto-plastic material    
    /// \param[in] id Material identification    
    /// \param[in] density Material density \f$\rho\f$
    /// \param[in] poisson_ Poisson's ratio \f$\nu\f$    
    /// \param[in] lambda_ Compresion slope index
    /// \param[in] kappa_ Sweelling slope index
    /// \param[in] Mc_ Slope of critical line (p-q) for compression
    /// \param[in] Me_ Slope of critical line (p-q) in extension 
    /// \param[in] nu0_ Initial specific volume \f$\nu_0\f$
    /// \param[in] initialp0_ Initial preconsolidation pressure \f$ p_0\f$

    CamClay(int id, double density, double poisson_, double lambda_, double kappa_, double Mc_, double Me_, double nu0_, double initialp0_);

    /// \brief Default destructor 
	virtual ~CamClay();

    /// \brief Initializes Cam-Clay internal variables
    /// \param[in] particle Particle
    void initializePoint(Particle* particle) const;
    
    /// \brief Update the stress in the particle
    /// \param[in] particle Particle
    virtual void updateStress(Particle* particle) const;

protected:
    // =====================================================
    // Material parameters
    // =====================================================
    double poisson; //!< Poisson's ratio \f$\nu\f$
    double lambda; //!< Compresion slope index
    double kappa; //!< Sweelling slope index
    double Mc; //!< Slope of critical line (p-q) for compression
    double Me; //!< Slope of critical line (p-q) in extension 
    double nu0; //!< Initial specific volume \f$\nu_0\f$
    double initialp0; //!< Initial preconsolidation pressure \f$p_0\f$

    // =====================================================
    // Internal variables
    // =====================================================
    enum class InternalVariable : std::size_t{p0 = 0, count}; //!< Preconsolidation pressure \f$ p_0\f$

    static constexpr std::size_t index(InternalVariable variable) { return static_cast<std::size_t>(variable);}

    // =====================================================
    // Precomputed constants
    // =====================================================
    double Kbar0; //!< \f$\bar{K}_0=\nu_0/\kappa\f$
    double Gbar0; //!< \f$\bar{G}_0= 3(1-2\nu)\bar{K}_0/[2(1+\nu)]\f$
    double C1; //!< \f$ C1=nu0/(lambda-kappa)\f$
    double n; //!< Ratio between slope of critical line for extension and compression \f$n=Me/Mc\f$

    // =====================================================
    // Numerical tolerances
    // =====================================================

    double invariantTolerance_;

    // =====================================================
    // Stress state
    // =====================================================
    struct StressState {
        Matrix3d stress;
        Matrix3d stressDev;
        double I;
        double J;
        double S;
        double alpha;
        double N;
        double gbar;
    };

    StressState computeStressState(const Matrix3d& stress) const;

};

#endif /* INC_MATERIALS_CAMCLAY_H_ */
