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

// Stress state: invariants
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

// Exact elastic properties
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

// Yield function
double CamClay::computeYieldFunction(const StressState& state, double p0) const
{
    return state.I * state.I + state.J * state.J /(state.N * state.N) - 3.0 * p0 * state.I;
}

// Yield gradient (r_ij)
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

// Exact hardening 
double CamClay::computeUpdatedPreconsolidationPressure(double p0Old, double plasticMultiplier, double rkk) const
{
    return p0Old * std::exp(C1 * plasticMultiplier * rkk);
}

// CPPM constitutive equations and residuals
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

// Second derivatives of the yield function required by CPPM

//A8.17
Matrix3d CamClay::computeAlphaStressDerivative(const StressState& state) const
{       
    const Matrix3d identity = Matrix3d::Identity();
    if (state.J <= zeroTolerance) {return Matrix3d::Zero();}
    const double cos3Alpha = std::cos(3.0 * state.alpha);
    if (std::abs(cos3Alpha) <= zeroTolerance) {return Matrix3d::Zero();}
    const double SOverJ = state.S / state.J;
    const Matrix3d bracket = (state.stressDev * state.stressDev) / (state.J * state.J)
    - (2.0 / 3.0) * identity
    - 1.5 * SOverJ * SOverJ * SOverJ * state.stressDev / state.J;

    return std::sqrt(3.0) / (2.0 * state.J* cos3Alpha) * bracket;
    
}

//A8.23
Matrix3d CamClay::computeYieldDerivativeJStressDerivative(const StressState& state) const
{
    if (state.J <= zeroTolerance) {return Matrix3d::Zero();}
    const double denominator_g = 1.0 + n - (1.0 - n) * std::sin(3.0 * state.alpha);
    const double g = 2.0 * n / denominator_g;
    const Matrix3d alphaStressDerivative = computeAlphaStressDerivative(state);
    const Matrix3d firstTerm = state.stressDev / (state.N * state.N * state.J);
    const Matrix3d secondTerm = 6.0 * state.J / (state.N * state.N)
    * (1.0 - n) / n * g *std::cos(3.0 * state.alpha) * alphaStressDerivative;

    return firstTerm - secondTerm;
}

//A8.21
Matrix3d CamClay::computeGbarStressDerivative(const StressState& state) const
{
    if (state.J <= zeroTolerance) {return Matrix3d::Zero();}
    const Matrix3d alphaStressDerivative = computeAlphaStressDerivative(state);
    const double sin3Alpha = std::sin(3.0 * state.alpha);
    const double cos3Alpha = std::cos(3.0 * state.alpha);
    const double denominator_g = 1.0 + n - (1.0 - n) * std::sin(3.0 * state.alpha);
    const double g = 2.0 * n / denominator_g;
    const double coefficient = 9.0 * (1 -n) / (2.0 * n) * g * sin3Alpha
     - 9.0 * (1.0 - n) * (1 - n) / (4.0 * n *n) * g * g * cos3Alpha * cos3Alpha;

    return coefficient * alphaStressDerivative;
}

//A8.24
Matrix3d CamClay::computeYieldDerivativeAlphaStressDerivative(const StressState& state) const
{
    if (state.J <= zeroTolerance) {return Matrix3d::Zero();}
    const double dPhi_dJ = 2.0 * state.J / (state.N * state.N);
    const Matrix3d dPhi_dJ_StressDerivative = computeYieldDerivativeJStressDerivative(state);
    const Matrix3d JStressDerivative = state.stressDev / (2.0 * state.J);
    const Matrix3d gbarStressDerivative = computeGbarStressDerivative(state);

    return state.J * state.gbar * dPhi_dJ_StressDerivative + state.gbar * dPhi_dJ * JStressDerivative + state.J * dPhi_dJ * gbarStressDerivative;

}

CamClay::Matrix6d CamClay::computeRStressDerivativeAlphaBoundary(double alpha) const
{
    Matrix6d drij_dStress = Matrix6d::Zero();
    // alpha = +pi/6/: triaxial compression -> Mc
    // alpha = -pi/6/: triaxial extension -> Me
    const double M = (alpha >= 0.0 ? Mc : Me);
    const double N = M / (3.0 * std::sqrt(3.0));
    const double inverseN2 = 1.0 / (N * N);
    // d_rij/dsigma_pq = 2 delta_ij delta_pq + 1/N^2 * (ds_ij/dsigma_pq)
    for (int column = 0; column < 6; ++column)
    {
        Matrix3d E = Matrix3d::Zero();
        if (column==0) {E(0, 0) = 1.0;}
        else if (column==1) {E(1, 1) = 1.0;}
        else if (column==2) {E(2, 2) = 1.0;}
        else if (column==3) {E(0, 1) = 1.0; E(1, 0) = 1.0;}
        else if (column==4) {E(1, 2) = 1.0; E(2, 1) = 1.0;}
        else {E(0, 2) = 1.0; E(2, 0) = 1.0;}
        const double dI = E.trace();
        const Matrix3d ds = E - dI / 3.0 * Matrix3d::Identity();
        const Matrix3d dr = 2.0 * dI * Matrix3d::Identity() + inverseN2 * ds;
        // Reduced gradient convetion used [r11 r22 r33 2r12 2r23 2r13]
        drij_dStress(0, column) = dr(0,0);
        drij_dStress(1, column) = dr(1,1);
        drij_dStress(2, column) = dr(2,2);
        drij_dStress(3, column) = 2 * dr(0,1);
        drij_dStress(4, column) = 2 * dr(1,2);
        drij_dStress(5, column) = 2 * dr(0,2);
    }
    return drij_dStress;
} 

CamClay::Matrix6d CamClay::computeRStressDerivative(const StressState& state) const
{
    const Matrix3d identity = Matrix3d::Identity();
    Matrix6d drij_dStress = Matrix6d::Zero();

    // Current stress invariants
    const Matrix3d& s = state.stressDev;
    const double J = state.J;
    const double S = state.S;
    const double alpha = state.alpha;
    if (J <= zeroTolerance) {throw std::runtime_error("Analytical dr/dsigma is not implemented for J = 0.");}
    const double cos3Alpha = std::cos(3.0 * alpha);
    if (std::abs(cos3Alpha) <= zeroTolerance) {return computeRStressDerivativeAlphaBoundary(alpha);}

    // First derivatives of the yield function
    const double dPhi_dJ = 2.0 * state.J / (state.N * state.N);
    const double dPhi_dAlpha = state.J * state.gbar * dPhi_dJ;

    // Stress derivatives of dPhi_dI, dPhi_dJ, dPhi_dAlpha
    const Matrix3d dPhi_dJ_dStress = computeYieldDerivativeJStressDerivative(state);
    const Matrix3d dPhi_dAlpha_dStress = computeYieldDerivativeAlphaStressDerivative(state);
    
    // a_ij, b_ij and c_ij Appendix 8. A8.25
    //a_ij = delta_ij
    const Matrix3d aij = identity;
    //b_ij = dJ/dsigma_ij = s_ij / (2J)
    const Matrix3d bij = s / (2.0 * J);
    //c_ij = dAlpha/dsigma_ij 
    const Matrix3d cij = computeAlphaStressDerivative(state);

    // Six independent stress directions
    for (int column = 0; column < 6; ++column)
    {
        Matrix3d E = Matrix3d::Zero();
        // Reduced ordering:
        // 0 -> sigma11
        // 1 -> sigma22
        // 2 -> sigma33
        // 3 -> sigma12 = sigma21
        // 4 -> sigma23 = sigma32
        // 5 -> sigma13 = sigma31
        if (column == 0) {E(0, 0) = 1.0;}
        else if (column == 1) {E(1, 1) = 1.0;}
        else if (column == 2) {E(2, 2) = 1.0;}
        else if (column == 3) {E(0, 1) = 1.0; E(1, 0) = 1.0;}
        else if (column == 4) {E(1, 2) = 1.0; E(2, 1) = 1.0;}
        else {E(0, 2) = 1.0; E(2, 0) = 1.0;}

        //A8.14: ds_ij/dsigma_pq
        //ds_ij = ds_ij/dsigma_pq * E_pq
        const Matrix3d ds = E - E.trace() / 3.0 * identity;
        //A8.15: dJ/dsigma_pq
        //dJ = dJ/dsigma_pq * ds_pq = dJ/dsigma_pq E_pq
        const double dJ = (s.array() * ds.array()).sum() / (2.0 * J);
        //A8.17: dalpha/dsigma_pq
        //dalpha = dalpha/dsigma_pq E_pq
        const double dAlpha = (cij.array() * E.array()).sum();
        //A8.22: d(dphi/dI)/dsimga_pq
        //d(dphi/dI) = d(dphi/dI)/dsimga_pq E_pq
        const double dPhiI = 2.0 * E.trace();
        //A8.23: d(dPhi/dJ)/dsigma_pq
        //d(dPhi/dJ) = d(dPhi/dJ)/dsigma_pq E_pq
        const double dPhiJ = (dPhi_dJ_dStress.array() * E.array()).sum();
        //A8.24: d(dPhi/dalpha)/dsigma_pq
        //d(dPhi/dalpha) = d(dPhi/dalpha)/dsigma_pq E_pq
        const double dPhiAlpha = (dPhi_dAlpha_dStress.array() * E.array()).sum();

        //A8.25b: L_ijpq^10 = 0
        //A8.26: L_ijpq^20 = db_ij/dsigma_pq 
        const Matrix3d L20 = ds / (2.0 * J) - s * dJ / (2.0 * J * J);

        //Qauntities needed for L_ijpq^30
        const Matrix3d sSquared = s * s;
        const double S3 = S * S * S;
        //a0 =  sqrt(3)/(2 cos(3 alpha))
        const double a0 = std::sqrt(3.0) / (2.0 * cos3Alpha);
        //aBar_ij = s_ir s_rj / J^3 - 2/(3J) delta_ij - 3/2 S^3/J^5 s_ij
        const Matrix3d aBarij = sSquared / (J * J * J) - 2.0 / (3.0 * J) * identity - 1.5 * S3 / (J * J * J * J *J) * s;
        // const Matrix3d cij = a0 * aBar_ij;
        // A.8.27: L_ijpq^30 = dc_ij/dsigma_pq = d(a0 * aBar_ij)/dsigma_pq = da0/dsigma_pq * * aBar_ij + * a0 * daBar_ij/dsigma_pq 
        //= da0/dsigma_pq * * aBar_ij + * a0 * d(s_ir s_rj / J^3 - 2/(3J) delta_ij - 3/2 S^3/J^5 s_ij)/dsigma_pq 
        
        //Derivative of first term of aBar_ij: d(s_ir s_rj / J^3)/dsigma_pq 
        const Matrix3d daBarFirst = (ds * s + s * ds) / (J * J * J) - 3.0 * sSquared * dJ / (J * J * J * J);
        //Derivative of second term of aBar_ij: d(- 2/(3J) delta_ij)/dsigma_pq 
        const Matrix3d daBarSecond = 2.0 / 3.0 * dJ / (J * J) * identity;
        //Derivative of third term of aBar_ij: d(- 3/2 S^3/J^5 s_ij)/dsigma_pq 
        //Contibution from J
        const Matrix3d daBarThirdFromJ = 7.5 * S3 * dJ / (J * J * J * J * J * J) * s;
        //Contribution from s_ij
        const Matrix3d daBarThirdFromsij = - 1.5 * S3 / (J * J * J * J * J) * ds;
        //Contribution from S
        //S^3 = 1/3 tr(s^3), then d(S^3) = tr(s^2 ds)
        const double dS3 = (sSquared * ds).trace();
        const Matrix3d daBarThirdFromS = - 1.5 * dS3 / (J * J * J * J * J) * s;
        //Total derivative of aBar_ij
        const Matrix3d daBar = daBarFirst + daBarSecond + daBarThirdFromJ + daBarThirdFromsij + daBarThirdFromS;

        //Derivative of a0: da0/dsigma_pq 
        const double da0 = 3.0 * a0 * std::tan(3.0 * alpha) * dAlpha;

        // A.8.27: L_ijpq^30 = dc_ij/dsigma_pq
        const Matrix3d L30 = da0 * aBarij + a0 * daBar;

        //A8.25a: L_ijpq^1 = d(dPhi/dI)/dsigma_pq * a_ij + dPhi/dI * L_ijpq^10 = d(dPhi/dI)/dsigma_pq * a_ij 
        const Matrix3d L1 = dPhiI * aij;
        //A8.25a: L_ijpq^2 = d(dPhi/dJ)/dsigma_pq * b_ij + dPhi/dJ * L_ijpq^20  
        const Matrix3d L2 = dPhiJ * bij + dPhi_dJ * L20;
        //A8.25a: L_ijpq^3 = d(dPhi/dalpha)/dsigma_pq * c_ij + dPhi/dalpha * L_ijpq^30 
        const Matrix3d L3 = dPhiAlpha * cij + dPhi_dAlpha * L30;

        //A8.25a dr_ij/dsigma_pq = L1 + L2 + L3
        const Matrix3d dr = L1 + L2 + L3;

        //Tensor -> reduced 6 vector
        // Reduced gradient convetion used [r11 r22 r33 2r12 2r23 2r13]
        
        drij_dStress(0, column) = dr(0,0);
        drij_dStress(1, column) = dr(1,1);
        drij_dStress(2, column) = dr(2,2);
        drij_dStress(3, column) = 2 * dr(0,1);
        drij_dStress(4, column) = 2 * dr(1,2);
        drij_dStress(5, column) = 2 * dr(0,2);
    }  
    return drij_dStress;
}
    

// Initialization
void CamClay::initializePoint(Particle* particle) const
{
    if (particle == nullptr){throw std::invalid_argument("The material-particle pointer is null.");}
    particle->initializeInternalVariables(index(InternalVariable::count));
    particle->setInternalVariable(index(InternalVariable::p0), initialp0);

}

// Stress update
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
	
    // Exact elastic trial state
    
    // get trial elastic stress
    Matrix3d trialStress = computeElasticTrialStress(oldState, de);
    StressState trialState = computeStressState(trialStress);

    // Evaluate yield function at trial state
    double phiTrial = computeYieldFunction(trialState, p0Old);
    const double phiScale = std::max({1.0, trialState.I * trialState.I, trialState.J * trialState.J, p0Old * p0Old});

    // Elastic step
    if (phiTrial <= TolPhi * phiScale) {particle->setStress(-trialStress); return;}
    

   
}
