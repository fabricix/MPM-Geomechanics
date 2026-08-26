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
    // Material parameters
    double poisson; //!< Poisson's ratio \f$\nu\f$
    double lambda; //!< Compresion slope index
    double kappa; //!< Sweelling slope index
    double Mc; //!< Slope of critical line (p-q) for compression
    double Me; //!< Slope of critical line (p-q) in extension 
    double nu0; //!< Initial specific volume \f$\nu_0\f$
    double initialp0; //!< Initial preconsolidation pressure \f$p_0\f$

    // Internal variables
    enum class InternalVariable : std::size_t{p0 = 0, count}; //!< Preconsolidation pressure \f$ p_0\f$

    static constexpr std::size_t index(InternalVariable variable) { return static_cast<std::size_t>(variable);}

    // Precomputed constants
    double Kbar0; //!< \f$\bar{K}_0=\nu_0/\kappa\f$
    double Gbar0; //!< \f$\bar{G}_0= 3(1-2\nu)\bar{K}_0/[2(1+\nu)]\f$
    double C1; //!< \f$ C1=nu0/(lambda-kappa)\f$
    double n; //!< Ratio between slope of critical line for extension and compression \f$n=Me/Mc\f$

    // Numerical tolerances
    double zeroTolerance; //!< Numerical tolerance for values close to zero. Machine-level tolerance (TOL)
    double TolStress; //!< Stress residual tolerance TOL_sigma
    double Tolp0; //!< Hardening residual tolerance TOL_zeta
    double TolPhi; //!< Yield residual tolerance TOL_phi
    int NLMax; //!< Maximum number of Newton iterations

    using Matrix6d = Eigen::Matrix<double, 6, 6>;
    using Vector6d = Eigen::Matrix<double, 6, 1>;
    using RowVector6d = Eigen::Matrix<double, 1, 6>;
    using Matrix8d = Eigen::Matrix<double, 8, 8>;
    using Vector8d = Eigen::Matrix<double, 8, 1>;

    // Stress state
    struct StressState {
        Eigen::Matrix3d stress;
        Eigen::Matrix3d stressDev;
        double I;
        double J;
        double S;
        double alpha;
        double N;
        double gbar;
    };

    StressState computeStressState(const Eigen::Matrix3d& stress) const;

    // Exact elastic properties
    Eigen::Matrix3d computeElasticTrialStress(const StressState& oldState, const Eigen::Matrix3d& de) const;

    // Yield gradient / flow direction
    Eigen::Matrix3d computeYieldGradient(const StressState& state, double p0) const;

    // CPPM constitutive equations
    Eigen::Matrix3d computeElastoplasticStress(const StressState& oldState, const Eigen::Matrix3d& de, const Eigen::Matrix3d& rij, double DeltaLambda) const;

    // Second derivatives of the yield function required by CPPM
    Eigen::Matrix3d computeAlphaStressDerivative(const StressState& state) const;
    Eigen::Matrix3d computeYieldDerivativeJStressDerivative(const StressState& state) const;
    Eigen::Matrix3d computeGbarStressDerivative(const StressState& state) const;
    Eigen::Matrix3d computeYieldDerivativeAlphaStressDerivative(const StressState& state) const;
    
    Matrix6d computeRStressDerivativeAlphaBoundary(double alpha) const;
    Matrix6d computeRStressDerivative(const StressState& state) const;

    struct CPPMCoefficients
    {
        Matrix6d A;
        Vector6d B;
        Vector6d F;
        RowVector6d H;
        double omega;
        double beta;
        RowVector6d E;
        double gamma;
    };

    //Complete CPPM integration

    CPPMCoefficients computeCPPMCoefficients(const StressState& oldState, const Eigen::Matrix3d& de, const StressState& currentState, const Eigen::Matrix3d& rij, double p0Old, double DeltaLambda) const;

    struct CPPMResult
    {
        Eigen::Matrix3d stress;
        double p0;
        double DeltaLambda;
        bool plastic;
    };

    double computeInitialPlasticMultiplier(const StressState& trialState, double phiTrial, const Eigen::Matrix3d& rTrial, double rkkTrial, double p0Old) const;

    //Main constitutive integrator
    CPPMResult solveCPPM(const Eigen::Matrix3d& stressOld, const Eigen::Matrix3d& de, double p0Old) const; 
    
    //Exact closed-form treatment for an isotropic plastic loading path J = 0
    CPPMResult solveIsotropicPlasticStep(const Eigen::Matrix3d& stressOld, const Eigen::Matrix3d& de, double p0Old) const;
        
};

#endif /* INC_MATERIALS_CAMCLAY_H_ */
