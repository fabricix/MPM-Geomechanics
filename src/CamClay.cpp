// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2026 MPM-Geomechanics Development Team


#include <Materials/CamClay.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>
// #include <Eigen/LU>
// using namespace Eigen;


CamClay::CamClay(int id, double density, double poisson_, double lambda_, double kappa_, double Mc_, double Me_, double nu0_, double initialp0_) 
: Material(id, density, Material::MaterialType::CAMCLAY)
{
    // Model parameters
    // poisson Poisson's ratio 
    // lambda Compresion slope index
    // kappa Sweelling slope index
    // Mc Slope of critical line (p-q) for compression
    // Me Slope of critical line (p-q) in extension 
    // nu0 Initial specific volume 
    // initialp0 Initial preconsolidation pressure

    this->poisson = poisson_;
    this->lambda = lambda_;
    this->kappa = kappa_;
    this->Mc = Mc_;
    this->Me = Me_;
    this->nu0 = nu0_;
    this->initialp0 = initialp0_;

    if (this->Mc <= 0.0) {throw std::invalid_argument("Mc must be positive.");}
    if (this->Me <= 0.0) {throw std::invalid_argument("Me must be positive.");}
    if (this->lambda <= 0.0) {throw std::invalid_argument("Lambda must be positive.");}
    if (this->kappa <= 0.0) {throw std::invalid_argument("Kappa must be positive.");}
    if (this->lambda <= this->kappa) {throw std::invalid_argument("Modified Cam-Clay requires lambda > kappa.");}
    if (this->poisson < 0.0 || this->poisson >= 0.5) {throw std::invalid_argument("Poisson's ratio must satisfy 0 <= nu < 0.5.");}
    if (this->nu0 <= 1.0) {throw std::invalid_argument("The initial specific volume must be greater than 1.");}
    if (this->initialp0 <= 0.0) {throw std::invalid_argument("The initial preconsolidation pressure must be positive.");}

    this->Kbar0 = this->nu0/this->kappa;
    this->Gbar0 = 3.0*(1.0-2.0*this->poisson)*this->Kbar0/(2.0*(1.0+this->poisson));
    this->C1 = this->nu0/(this->lambda-this->kappa);
    this->n=this->Me / this->Mc;

    this->zeroTolerance = 1.0e-12;
    this->TolPhi = 1.0e-10;
}

CamClay::~CamClay() { }

// =========================================================
// Stress state: invariants
// =========================================================

CamClay::StressState CamClay::computeStressState(const Matrix3d& stress) const
{
    StressState state;
    state.stress = stress;
    state.I = stress.trace();
    state.stressDev = stress - state.I / 3.0 * Matrix3d::Identity();
    state.J = std::sqrt(0.5 * state.stressDev.squaredNorm());
    const Matrix3d sSquared = state.stressDev * state.stressDev;
    state.S = std::cbrt((sSquared * state.stressDev).trace()/ 3.0);
    // state.S = state.stressDev.determinant();
    if (state.J <= zeroTolerance) {state.alpha = 0.0;}
    else{
        const double ratioSJ = state.S/state.J;
        double argumentAlpha = 3.0 * std::sqrt(3.0) / 2.0 * ratioSJ * ratioSJ * ratioSJ;
        argumentAlpha = std::clamp(argumentAlpha, -1.0, 1.0);
        state.alpha = std::asin(argumentAlpha) / 3.0;
    }
    const double denominator_g = 1.0 + n - (1.0 - n) * std::sin(3.0 * state.alpha);
    if (std::abs(denominator_g) <= zeroTolerance) {throw std::runtime_error("Singular critical state surface denominator of g.");}
    const double g = 2.0 * n / denominator_g;
    state.N = g * Mc / (3.0 * std::sqrt(3.0));
    if(std::abs(state.N) <= zeroTolerance) {throw std::runtime_error("Singular ratio N");}
    state.gbar = -3.0 * (1.0 - n) * std::cos(3.0 * state.alpha) / denominator_g;

    return state;    
}

//=====================================================
// Exact elastic properties
//=====================================================

Matrix3d CamClay::computeElasticTrialStress(const StressState& oldState, const Matrix3d& de) const
{
    // strain increment and its deviator
    const Matrix3d deDev = de - Matrix3d::Identity()*de.trace()/3.0;
    const double deVol = de.trace();

    // current stress and deviate
    const double pOld = oldState.I / 3.0;
    if (pOld <= 0.0) {throw std::runtime_error("Cam-Clay requires a positive compressive mean pressure");}
    const double x = Kbar0 * deVol;
    const double Gave = (std::abs(x) <= zeroTolerance) ? Gbar0 * pOld : Gbar0 * pOld * std::expm1(x) / x;
    const double pTrial = pOld * std::exp(x);
    return oldState.stressDev + 2.0 * Gave * deDev + pTrial * Matrix3d::Identity();
}

//=====================================================
// Yield function
//=====================================================
   
double CamClay::computeYieldFunction(const StressState& state, double p0) const
{
    return state.I * state.I + state.J * state.J /(state.N * state.N) - 3.0 * p0 * state.I;
}

// =========================================================
// Yield gradient (r_ij)
// =========================================================

Matrix3d CamClay::computeYieldGradient(const StressState& state, double p0) const
{   
    const Matrix3d identity = Matrix3d::Identity();
    // Contribution through I
    const double dPhi_dI = 2.0 * state.I - 3.0 * p0;
    Matrix3d rij = dPhi_dI * identity;
    // J = 0: only volumetric contribution remains; the J and alpha contributions are neglected.
    if (state.J <= zeroTolerance) {return rij;}
    // Contribution through J
    const double dPhi_dJ = 2.0 * state.J / (state.N * state.N);
    rij += dPhi_dJ * state.stressDev / (2.0 * state.J);
    // alpha = +/- pi/6: cos (3 alpha) = 0.0; the alpha contribution is neglected.
    const double cos3Alpha = std::cos(3.0 * state.alpha);
    if (std::abs(cos3Alpha) <= zeroTolerance) {return rij;}
    // Contribution through alpha
    const double dPhi_dAlpha = state.J * state.gbar * dPhi_dJ;
    const double SOverJ = state.S / state.J;
    const Matrix3d bracket = (state.stressDev * state.stressDev) / (state.J * state.J)
    - (2.0 / 3.0) * identity
    - 1.5 * SOverJ * SOverJ * SOverJ * state.stressDev / state.J;
    rij += dPhi_dAlpha * std::sqrt(3.0) / (2.0 * state.J * cos3Alpha) * bracket;

    return rij;
}

// =========================================================
// Exact hardening 
// =========================================================

double CamClay::computeUpdatedPreconsolidationPressure(double p0Old, double plasticMultiplier, double rkk) const
{
    return p0Old * std::exp(C1 * plasticMultiplier * rkk);
}

// =========================================================
// CPPM constitutive equations and residuals
// =========================================================

Matrix3d CamClay::computeElastoplasticStress(const StressState& oldState, const Matrix3d& de, const Matrix3d& rij, double plasticMultiplier) const
{
    Matrix3d identity = Matrix3d::Identity();
    // State at n
    const double pOld = oldState.I / 3.0;
    if (pOld <= zeroTolerance) {throw std::runtime_error("The old mean pressure must be positive.");}
    // Strain decomposition
    const double deVol = de.trace();
    const Matrix3d deDev = de - deVol / 3.0 * identity;
    // Flow direction evaluated at current Newton estimate
    const double rkk = rij.trace();
    const Matrix3d dij = rij - rkk / 3.0 * identity;
    const double barX0 = Kbar0 * (deVol - plasticMultiplier * rkk);
    const Matrix3d DeltaY = deDev - plasticMultiplier * dij;
    const double Gave = (std::abs(barX0) <= zeroTolerance) ? Gbar0 * pOld : Gbar0 * pOld * std::expm1(barX0) / barX0;

    return oldState.stressDev + 2.0 * Gave * DeltaY + pOld * std::exp(barX0) * identity;

}
Matrix3d CamClay::computeStressResidual(const StressState& oldState, const Matrix3d& de, const StressState& currentState, const Matrix3d& rij, double plasticMultiplier) const
{
    const Matrix3d stressNew = computeElastoplasticStress(oldState, de, rij, plasticMultiplier);
    return currentState.stress - stressNew;
    }
double CamClay::computeHardeningResidual(double p0Old, double p0Current, double plasticMultiplier, double rkk) const
{
    const double p0New = computeUpdatedPreconsolidationPressure(p0Old,plasticMultiplier,rkk);
    return p0Current - p0New;
}

// =========================================================
// Initialization
// =========================================================

void CamClay::initializePoint(Particle* particle) const
{
    if (particle == nullptr){throw std::invalid_argument("The material-particle pointer is null.");}
    particle->initializeInternalVariables(index(InternalVariable::count));
    particle->setInternalVariable(index(InternalVariable::p0), initialp0);

}

// =========================================================
// Stress update
// =========================================================
void CamClay::updateStress(Particle *particle) const
{
    if (particle == nullptr){throw std::invalid_argument("The material-particle pointer is null.");}

    // =====================================================
    // Sign convention
    // MPM-Geomechanics: compression = negative
    // Anandarajah implementation: compression = positive
    // =====================================================
    
    // Stress at time n from Particle
	Matrix3d stressOld = -particle->getStress();

    StressState oldState = computeStressState(stressOld);
	
    // Strain increment 
	Matrix3d de = -particle->getStrainIncrement();

    // Preconsolidation pressure at time n from Particle
    double p0Old = particle->getInternalVariable(index(InternalVariable::p0));
	
    // =====================================================
    // Exact elastic trial state
    // =====================================================

    // get trial elastic stress
    Matrix3d trialStress = computeElasticTrialStress(oldState, de);
    StressState trialState = computeStressState(trialStress);

    // Evaluate yield function at trial state
    double phiTrial = computeYieldFunction(trialState, p0Old);
    const double phiScale = std::max({1.0, trialState.I * trialState.I, trialState.J * trialState.J, p0Old * p0Old});

    // Elastic step
    if (phiTrial <= TolPhi * phiScale) {particle->setStress(-trialStress); return;}
    

   
}
