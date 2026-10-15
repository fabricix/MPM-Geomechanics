// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2026 MPM-Geomechanics Development Team

#ifndef INC_MATERIALS_CAMCLAY_H_
#define INC_MATERIALS_CAMCLAY_H_

#include "Materials/Material.h"
#include "Particle/Particle.h"

#include "Eigen/Core"

#include <cstddef>

/// \class CamClay
/// \brief Represents an elasto-plastic material using the CamClay failure criteria 
class CamClay: public Material {
public:

    /// \brief Create a CamClay elasto-plastic material    
    /// \param[in] id Material identification    
    /// \param[in] density Material density \f$\rho\f$
    /// \param[in] poisson_ Poisson's ratio \f$\nu\f$    
    /// \param[in] lambda_ Compression slope index
    /// \param[in] kappa_ Swelling slope index
    /// \param[in] Mc_ Slope of critical line (p-q) for compression
    /// \param[in] Me_ Slope of critical line (p-q) in extension 
    /// \param[in] nu0_ Initial specific volume \f$\nu_0\f$
    /// \param[in] initialp0_ Initial preconsolidation pressure \f$ p_0\f$

    CamClay(int id, double density, double poisson_, double lambda_, double kappa_, double Mc_, double Me_, double nu0_, double initialp0_);

    /// \brief Default destructor 
	virtual ~CamClay();

    /// \brief Initializes Cam-Clay internal variables and validates the initial stress state of a particle
    /// \param[in] particle Particle
    void initializeParticle(Particle* particle) const;
    
    /// \brief Update the stress in the particle
    /// \param[in] particle Particle
    virtual void updateStress(Particle* particle) const override;

    /// \brief Return a reference sound speed for Courant-Friedrichs-Lewy condition estimation
	/// \return sound_speed Sound speed evaluated at the initial preconsolidation pressure
    virtual double getSoundSpeed() const override;

protected:
    // Material parameters
    double poisson; //!< Poisson's ratio \f$\nu\f$
    double lambda; //!< Compression slope index
    double kappa; //!< Swelling slope index
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
    double Bbar; //!< \f$\bar{B}=3\sqrt{3}(1-n)\f$, coefficient of the Lode-angle term in r_ij

    // Numerical parameters (all tolerances are dimensionless)
    double zeroTolerance;          //!< Relative tolerance for vanishing quantities (J/p, cos 3alpha, phi/p0^2)
    double isotropicPathTolerance; //!< Relative tolerance to detect a purely isotropic step (J/p, |de_dev|/|de_vol|)
    double TolStress;              //!< Newton tolerance on the stress residual, relative to |sigma|
    double Tolp0;                  //!< Newton tolerance on the hardening residual, relative to p0
    double TolPhi;                 //!< Newton tolerance on the yield residual, relative to p0^2
    int NLMax;                     //!< Maximum number of Newton iterations
    int maxSubStepLevel;           //!< Maximum bisection level of the strain increment (2^level substeps)

    using Matrix6d = Eigen::Matrix<double, 6, 6>;
    using Vector6d = Eigen::Matrix<double, 6, 1>;
    using RowVector6d = Eigen::Matrix<double, 1, 6>;
    using Matrix8d = Eigen::Matrix<double, 8, 8>;
    using Vector8d = Eigen::Matrix<double, 8, 1>;

    // Stress state
    /// \brief Stress tensor and the invariants used by the model
    struct StressState {
        Eigen::Matrix3d stress;    //!< Stress tensor \f$\sigma_{ij}\f$
        Eigen::Matrix3d stressDev; //!< Deviatoric stress \f$s_{ij}\f$
        double I;                  //!< First invariant \f$I=\sigma_{kk}=3p\f$
        double J;                  //!< \f$J=\sqrt{s_{ij}s_{ij}/2}\f$
        double S3;                 //!< \f$S^3=s_{ij}s_{jk}s_{ki}/3=\det(s)\f$
        double sin3Alpha;          //!< \f$\sin 3\alpha=(3\sqrt{3}/2)S^3/J^3\f$ (0 on the hydrostatic axis)
        double cos3Alpha;          //!< \f$\cos 3\alpha\geq 0\f$
        double alpha;              //!< Lode angle \f$\alpha\in[-\pi/6,\pi/6]\f$
        double denominator_g;      //!< \f$1+n-(1-n)\sin 3\alpha\f$, such that \f$g=2n/\mathrm{den}\f$
        double N;                  //!< \f$N=g M_c/(3\sqrt{3})\f$
        double gbar;               //!< \f$\bar{g}=-3(1-n)\cos 3\alpha/\mathrm{den}\f$
        bool hydrostatic;          //!< True when \f$J\leq\f$ zeroTolerance \f$|p|\f$
    };

    /// \brief Compute the invariants of a stress tensor
    StressState computeStressState(const Eigen::Matrix3d& stress) const;

    /// \brief Yield function \f$\phi=I^2+J^2/N^2-3p_0 I\f$
    double computeYieldFunction(const StressState& state, double p0) const;

    // Exact integration of the hypoelastic law (Anandarajah 12.53-12.56)
    /// \brief Quantities of the exact elastic update over one strain increment
    struct ElasticUpdate {
        Eigen::Matrix3d stress; //!< Updated stress
        Eigen::Matrix3d dij;    //!< Deviatoric part of the flow direction
        Eigen::Matrix3d DeltaY; //!< Elastic deviatoric strain increment \f$\Delta y_{ij}\f$ (12.55b)
        double pOld;            //!< Mean pressure at the beginning of the step
        double rkk;             //!< Trace of the flow direction
        double barX0;           //!< \f$\bar{x}_0=\bar{K}_0(\Delta\varepsilon_v-\Delta\lambda r_{kk})\f$ (12.55a)
        double expX0;           //!< \f$e^{\bar{x}_0}\f$
        double Gave;            //!< Average shear modulus along the step (12.56b)
    };

    /// \brief Exact elastic update with the plastic strain \f$\Delta\lambda r_{ij}\f$ removed
    /// (rij = 0, DeltaLambda = 0 gives the elastic predictor)
    ElasticUpdate computeElasticUpdate(const StressState& oldState, const Eigen::Matrix3d& de, const Eigen::Matrix3d& rij, double DeltaLambda) const;

    // Yield gradient (flow direction) and its stress derivative

    /// \brief Yield gradient \f$r_{ij}=\partial\phi/\partial\sigma_{ij}\f$ (regularized form of A8.8)
    Eigen::Matrix3d computeYieldGradient(const StressState& state, double p0) const;

    // Second derivatives of the yield function required by CPPM
    /// \brief A8.17: \f$c_{ij}=\partial\alpha/\partial\sigma_{ij}\f$
    Eigen::Matrix3d computeAlphaStressDerivative(const StressState& state) const;
    /// \brief A8.23: \f$\partial(\partial\phi/\partial J)/\partial\sigma_{ij}\f$
    Eigen::Matrix3d computeYieldDerivativeJStressDerivative(const StressState& state, const Eigen::Matrix3d& cij) const;
    /// \brief A8.21: \f$\partial\bar{g}/\partial\sigma_{ij}\f$
    Eigen::Matrix3d computeGbarStressDerivative(const StressState& state, const Eigen::Matrix3d& cij) const;
    /// \brief A8.24: \f$\partial(\partial\phi/\partial\alpha)/\partial\sigma_{ij}\f$
    Eigen::Matrix3d computeYieldDerivativeAlphaStressDerivative(const StressState& state, const Eigen::Matrix3d& cij, const Eigen::Matrix3d& dPhi_dJ_dStress) const;

    /// \brief A8.25: \f$\partial r_{ij}/\partial\sigma_{pq}\f$, dispatching to the analytical limits when needed
    Matrix6d computeRStressDerivative(const StressState& state) const;                // A8.25
    /// \brief Limit of A8.25 on the hydrostatic axis (J = 0)
    Matrix6d computeRStressDerivativeHydrostatic(const StressState& state) const;     // J = 0
    /// \brief Limit of A8.25 on the triaxial meridians (alpha = +/- pi/6)
    Matrix6d computeRStressDerivativeAlphaBoundary(const StressState& state) const;   // alpha = +/- pi/6

    // Helpers for the column-by-column construction of 6x6 stress derivatives
    // Conversions between symmetric tensors and 6-component vectors
    // Ordering: 0 -> 11, 1 -> 22, 2 -> 33, 3 -> 12, 4 -> 23, 5 -> 13
    /// \brief Unit symmetric tensor associated with one independent component
    static Eigen::Matrix3d unitStressDirection(int column);
    /// \brief Store dr as a reduced gradient column [r11 r22 r33 2r12 2r23 2r13]
    static void storeReducedColumn(Matrix6d& matrix, int column, const Eigen::Matrix3d& dr);
    /// \brief Recover the tensor from a reduced gradient column [r11 r22 r33 2r12 2r23 2r13]
    static Eigen::Matrix3d reducedColumnToTensor(const Matrix6d& matrix, int column);
    /// \brief Tensor components [t11 t22 t33 t12 t23 t13]
    static Vector6d tensorToVector(const Eigen::Matrix3d& tensor);
    /// \brief Symmetric tensor from components [t11 t22 t33 t12 t23 t13]
    static Eigen::Matrix3d vectorToTensor(const Vector6d& vector);

    // Closest point projection method (CPPM)

    /// \brief Blocks of the Newton Jacobian (Anandarajah 12.47, 12.63a-h)
    struct CPPMCoefficients
    {
        Matrix6d A;      //!< dR_sigma/dsigma
        Vector6d B;      //!< dR_sigma/dp0
        Vector6d F;      //!< dR_sigma/dDeltaLambda
        RowVector6d H;   //!< dR_p0/dsigma
        double omega;    //!< dR_p0/dp0
        double beta;     //!< dR_p0/dDeltaLambda
        RowVector6d E;   //!< dphi/dsigma
        double gamma;    //!< dphi/dp0
    };

    //Complete CPPM integration
    /// \brief Coefficients 12.63a-h at the current Newton estimate
    CPPMCoefficients computeCPPMCoefficients(const ElasticUpdate& update, const StressState& currentState, const Eigen::Matrix3d& rij, double p0Old, double DeltaLambda) const;

    /// \brief Result of the constitutive integration
    struct CPPMResult
    {
        Eigen::Matrix3d stress; //!< Stress at n+1
        double p0;              //!< Preconsolidation pressure at n+1
        double DeltaLambda;     //!< Plastic multiplier of the step
        bool plastic;           //!< True if the step was plastic
    };

    /// \brief 12.49: initial estimate of the plastic multiplier
    double computeInitialPlasticMultiplier(const StressState& trialState, double phiTrial, const Eigen::Matrix3d& rTrial, double p0Old) const;

    /// \brief Main constitutive integrator: elastic predictor, isotropic closed form,
    /// CPPM Newton and, if Newton fails, recursive bisection of the strain increment
    CPPMResult solveCPPM(const Eigen::Matrix3d& stressOld, const Eigen::Matrix3d& de, double p0Old, int subStepLevel = 0) const;

    /// \brief Newton-Raphson iterations of the CPPM
    /// \return false if Newton does not converge or converges to a non-physical state
    bool solveReturnMapping(const StressState& oldState, const Eigen::Matrix3d& de, double p0Old, const StressState& trialState, double phiTrial, CPPMResult& result) const;

    //Exact closed-form treatment for an isotropic plastic loading path J = 0
    CPPMResult solveIsotropicPlasticStep(const StressState& oldState, const Eigen::Matrix3d& de, double p0Old) const;
        
};

#endif /* INC_MATERIALS_CAMCLAY_H_ */
