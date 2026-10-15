// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2026 MPM-Geomechanics Development Team


#include "Materials/CamClay.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "Eigen/LU"
using Eigen::Matrix3d;


CamClay::CamClay(int id, double density, double poisson_, double lambda_, double kappa_, double Mc_, double Me_, double nu0_, double initialp0_) 
: Material(id, density, Material::MaterialType::CAMCLAY)
{
    // Model parameters
    // poisson Poisson's ratio 
    // lambda Compression slope index
    // kappa Swelling slope index
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
    this->n = this->Me / this->Mc;
    this->Bbar = 3.0 * std::sqrt(3.0) * (1.0 - this->n); 

    //Numerical parameters
    this->zeroTolerance = 1.0e-10;
    this->isotropicPathTolerance = 1.0e-8;
    this->TolStress = 1.0e-10;
    this->Tolp0 = 1.0e-10;
    this->TolPhi = 1.0e-10;
    this->NLMax = 50;
    this->maxSubStepLevel = 10;
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
    // S^3 = s_ij s_jk s_ki / 3 = det(s)
    state.S3 = (state.stressDev * state.stressDev * state.stressDev).trace() / 3.0;
    // Hydrostatic axis: J negligible with respect to the mean pressure
    state.hydrostatic = (state.J <= zeroTolerance * std::abs(state.I) / 3.0);
    // Lode angle: sin(3 alpha) = 3 sqrt(3)/2 S^3/J^3 (alpha = 0 on hydrostatic axis)
    state.sin3Alpha = 0.0;
    if (!state.hydrostatic) 
    {
        const double J3 = state.J * state.J * state.J;
        state.sin3Alpha = std::clamp(1.5 * std::sqrt(3.0) * state.S3 / J3, -1.0, 1.0);
    }
    // 3 alpha in (-pi/2, pi/2), cos(3 alpha) >= 0
    state.cos3Alpha = std::sqrt(std::max(0.0, 1.0 - state.sin3Alpha * state.sin3Alpha));
    state.alpha = std::asin(state.sin3Alpha) / 3.0;
    // Deviatoric shape g = 2 n / den, N = g Mc / (3 sqrt(3)), gbar = -(1/g) dg/dalpha
    state.denominator_g = 1.0 + n - (1.0 - n) * state.sin3Alpha;
    const double g = 2.0 * n / state.denominator_g;
    state.N = g * Mc / (3.0 * std::sqrt(3.0));
    state.gbar = -3.0 * (1.0 - n) * state.cos3Alpha / state.denominator_g;

    return state;    
}

double CamClay::computeYieldFunction(const StressState& state, double p0) const
{
    return state.I * state.I + state.J * state.J / (state.N * state.N) - 3.0 * p0 * state.I;
}

// Exact elastic properties
CamClay::ElasticUpdate CamClay::computeElasticUpdate(const StressState& oldState, const Eigen::Matrix3d& de, const Eigen::Matrix3d& rij, double DeltaLambda) const
{
    const Matrix3d identity = Matrix3d::Identity();
    ElasticUpdate update;

    //State at n
    update.pOld = oldState.I / 3.0;
    // strain increment and its deviator
    const double deVol = de.trace(); 
    const Matrix3d deDev = de - deVol / 3.0 * identity;
    // Flow direction decomposition
    update.rkk = rij.trace();
    update.dij = rij - update.rkk / 3.0 * identity;
    // 12.55a, 12.55b: elastic volumetric (scaled) and deviatoric strain increments
    update.barX0 = Kbar0 * (deVol - DeltaLambda * update.rkk);
    update.DeltaY = deDev - DeltaLambda * update.dij;
    update.expX0 = std::exp(update.barX0);
    // 12.56b: average shear modulus Gave = Gbar0 pOld (e^x - 1)/x (expm1 is accurate for small x)
    const double x = update.barX0;
    update.Gave = Gbar0 * update.pOld * ((x == 0.0) ? 1.0 : std::expm1(x) / x);
    // 12.53: sigma = s^n + 2 Gave Delta y + p^n e^x delta
    update.stress = oldState.stressDev + 2.0 * update.Gave * update.DeltaY + update.pOld * update.expX0 * identity;

    return update;
}

// Yield gradient (r_ij)
// Regularized form of A8.8: the 1/J and 1/cos(3 alpha) factors cancel analytically
// r_ij = (2I - 3p0) delta_ij + s_ij / N^2 - Bbar / (den N^2) * bracket_ij
Matrix3d CamClay::computeYieldGradient(const StressState& state, double p0) const
{   
    const Matrix3d identity = Matrix3d::Identity();
    const Matrix3d& s = state.stressDev;
    const double J = state.J;
    const double inverseN2 = 1.0 / (state.N * state.N);
    
    // Contributions through I and J: (2I - 3p0) delta_ij + s_ij / N^2
    Matrix3d rij = (2.0 * state.I - 3.0 * p0) * identity + inverseN2 * s;
    // Contribution through alpha is O(J): it vanishes continuously on the hydrostatic axis
    if (state.hydrostatic) {return rij;}
    
    const double q3 = state.S3 / (J * J * J);
    // bracket = s_ik s_kj / J - 2/3 J delta_ij - 3/2 (S/J)^3 s_ij   (traceless, O(J))
    const Matrix3d bracket = s * s / J 
    - (2.0 / 3.0) * J * identity
    - 1.5 * q3 * s;
    // alpha-term: -3 sqrt(3) (1 - n) / (den N^2) * bracket
    rij -= Bbar * inverseN2 / state.denominator_g * bracket;

    return rij;
}

// Second derivatives of the yield function required by CPPM
//A8.17: c_ij = dalpha/dsigma_ij = sqrt(3) / (2 J^2 cos(3 alpha)) * bracket_ij
Matrix3d CamClay::computeAlphaStressDerivative(const StressState& state) const
{       
    const Matrix3d identity = Matrix3d::Identity();
    const Matrix3d& s = state.stressDev;
    const double J = state.J;
    const double q3 = state.S3 / (J * J * J);
    // bracket = s_ik s_kj / J - 2/3 J delta_ij - 3/2 (S/J)^3 s_ij   (traceless, O(J))
    const Matrix3d bracket = s * s / J 
    - (2.0 / 3.0) * J * identity
    - 1.5 * q3 * s;

    //A8.17: c_ij = dalpha/dsigma_ij = sqrt(3) / (2 J^2 cos(3 alpha)) * bracket_ij
    // with bracket_ij = s_ik s_kj / J - 2/3 J delta_ij - 3/2 (S/J)^3 s_ij  (regularized, = J times the A8.17 bracket)
    return std::sqrt(3.0) / (2.0 * J * J * state.cos3Alpha) * bracket;
    
}

//A8.23: d(dPhi/dJ)/dsigma_ij = s_ij / (N^2 J) - 6 J/N^2 (1-n)/n g cos(3 alpha) c_ij
Matrix3d CamClay::computeYieldDerivativeJStressDerivative(const StressState& state, const Matrix3d& cij) const
{
    const double g = 2.0 * n / state.denominator_g;
    const double inverseN2 = 1.0 / (state.N * state.N);
    const Matrix3d firstTerm = state.stressDev * inverseN2/ state.J;
    const Matrix3d secondTerm = 6.0 * state.J *inverseN2
    * (1.0 - n) / n * g * state.cos3Alpha * cij;

    return firstTerm - secondTerm;
}

//A8.21: dgbar/dsigma_ij = [9(1-n)/(2n) g sin(3 alpha) - 9(1-n)^2/(4n^2) g^2 cos^2(3 alpha)] c_ij
Matrix3d CamClay::computeGbarStressDerivative(const StressState& state,  const Matrix3d& cij) const
{
    const double g = 2.0 * n / state.denominator_g;
    const double coefficient = 9.0 * (1 - n) / (2.0 * n) * g * state.sin3Alpha
     - 9.0 * (1.0 - n) * (1 - n) / (4.0 * n *n) * g * g * state.cos3Alpha * state.cos3Alpha;

    return coefficient * cij;
}

//A8.24: d(dPhi/dalpha)/dsigma_ij, with dPhi/dalpha = J gbar dPhi/dJ
Matrix3d CamClay::computeYieldDerivativeAlphaStressDerivative(const StressState& state, const Matrix3d& cij, const Matrix3d& dPhi_dJ_dStress) const
{
    const double dPhi_dJ = 2.0 * state.J / (state.N * state.N);
    const Matrix3d JStressDerivative = state.stressDev / (2.0 * state.J);
    const Matrix3d gbarStressDerivative = computeGbarStressDerivative(state, cij);

    return state.J * state.gbar * dPhi_dJ_dStress 
    + state.gbar * dPhi_dJ * JStressDerivative 
    + state.J * dPhi_dJ * gbarStressDerivative;

}

// Stress derivative of the flow direction dr_ij/dsigma_pq (Anandarajah A8.25)
// A8.25: dr_ij/dsigma_pq = L1 + L2 + L3,
// L_k = d(dPhi/dx_k)/dsigma_pq * a^k_ij + dPhi/dx_k * dа^k_ij/dsigma_pq,  x = (I, J, alpha)
// Reduced gradient convention: rows [r11 r22 r33 2r12 2r23 2r13], columns [s11 s22 s33 s12 s23 s13]
CamClay::Matrix6d CamClay::computeRStressDerivative(const StressState& state) const
{
    // Branch 1: J ~ 0 (hydrostatic axis). b_ij and c_ij are undefined; use the analytical limit
    if (state.hydrostatic) {return computeRStressDerivativeHydrostatic(state);}
    // Branch 2: cos(3 alpha) ~ 0 (triaxial meridians). c_ij and a0 are 0/0; use the analytical limit
    if (state.cos3Alpha <= zeroTolerance) {return computeRStressDerivativeAlphaBoundary(state);}
    
    const Matrix3d identity = Matrix3d::Identity();
    Matrix6d drij_dStress = Matrix6d::Zero();

    // Current stress invariants
    const Matrix3d& s = state.stressDev;
    const double J = state.J;
    const double S3 = state.S3;
    const Matrix3d sSquared = s * s;

    // First derivatives of the yield function
    const double dPhi_dJ = 2.0 * J / (state.N * state.N);
    const double dPhi_dAlpha = J * state.gbar * dPhi_dJ;

    // a_ij = dI/dsigma_ij, b_ij = dJ/dsigma_ij, c_ij = dalpha/dsigma_ij (computed once)
    //a_ij = delta_ij
    const Matrix3d aij = identity;
    //b_ij = dJ/dsigma_ij = s_ij / (2J)
    const Matrix3d bij = s / (2.0 * J);
    //c_ij = dAlpha/dsigma_ij 
    const Matrix3d cij = computeAlphaStressDerivative(state);

    // Stress derivatives of dPhi/dJ and dPhi/dalpha (A8.23, A8.24)
    const Matrix3d dPhi_dJ_dStress = computeYieldDerivativeJStressDerivative(state, cij);
    const Matrix3d dPhi_dAlpha_dStress = computeYieldDerivativeAlphaStressDerivative(state, cij, dPhi_dJ_dStress);
    
    // Quantities for L^30: c_ij = a0 * aBar_ij
    // c_ij = a0 aBar_ij, a0 = sqrt(3)/(2 cos(3 alpha)), aBar_ij = s_ir s_rj/J^3 - 2/(3J) delta_ij - 3/2 S^3/J^5 s_ij
    const double a0 = std::sqrt(3.0) / (2.0 * state.cos3Alpha);
    const double tan3Alpha = state.sin3Alpha / state.cos3Alpha;
    const double J2 = J * J;
    const double J3 = J2 * J;
    const double J5 = J3 *J2;
    const Matrix3d aBarij = sSquared / J3 - 2.0 / (3.0 * J) * identity - 1.5 * S3 / J5 * s;


    // Six independent stress directions
    for (int column = 0; column < 6; ++column)
    {
        const Matrix3d E_pq = unitStressDirection(column);

        //A8.14: ds_ij/dsigma_pq
        //ds_ij = ds_ij/dsigma_pq * E_pq
        const Matrix3d ds = E_pq  - E_pq.trace() / 3.0 * identity;
        //A8.15: dJ/dsigma_pq
        //dJ = dJ/dsigma_pq * ds_pq = dJ/dsigma_pq E_pq
        const double dJ = (s.array() * ds.array()).sum() / (2.0 * J);
        //A8.17: dalpha/dsigma_pq
        //dalpha = dalpha/dsigma_pq E_pq
        const double dAlpha = (cij.array() * E_pq.array()).sum();
        //A8.22: d(dphi/dI)/dsimga_pq
        //d(dphi/dI) = d(dphi/dI)/dsimga_pq E_pq
        const double dPhiI = 2.0 * E_pq.trace();
        //A8.23: d(dPhi/dJ)/dsigma_pq
        //d(dPhi/dJ) = d(dPhi/dJ)/dsigma_pq E_pq
        const double dPhiJ = (dPhi_dJ_dStress.array() * E_pq.array()).sum();
        //A8.24: d(dPhi/dalpha)/dsigma_pq
        //d(dPhi/dalpha) = d(dPhi/dalpha)/dsigma_pq E_pq
        const double dPhiAlpha = (dPhi_dAlpha_dStress.array() * E_pq.array()).sum();

        //A8.25b: L_ijpq^10 = 0
        //A8.26: L_ijpq^20 = db_ij/dsigma_pq 
        const Matrix3d L20 = ds / (2.0 * J) - s * dJ / (2.0 * J2);

        // A.8.27: L_ijpq^30 = dc_ij/dsigma_pq = d(a0 * aBar_ij)/dsigma_pq = da0/dsigma_pq * * aBar_ij + * a0 * daBar_ij/dsigma_pq 
        //= da0/dsigma_pq * * aBar_ij + * a0 * d(s_ir s_rj / J^3 - 2/(3J) delta_ij - 3/2 S^3/J^5 s_ij)/dsigma_pq         
        //Derivative of first term of aBar_ij: d(s_ir s_rj / J^3)/dsigma_pq 
        const Matrix3d daBarFirst = (ds * s + s * ds) / J3 - 3.0 * sSquared * dJ / (J3 * J);
        //Derivative of second term of aBar_ij: d(- 2/(3J) delta_ij)/dsigma_pq 
        const Matrix3d daBarSecond = (2.0 / 3.0) * dJ / J2 * identity;
        //S^3 = 1/3 tr(s^3), then d(S^3) = tr(s^2 ds)
        const double dS3 = (sSquared * ds).trace();
        //Derivative of third term of aBar_ij: d(- 3/2 S^3/J^5 s_ij)/dsigma_pq 
        //Contibution from J, s_ij and S 
        const Matrix3d daBarThird = 7.5 * S3 * dJ / (J5 * J) * s - 1.5 * S3 / J5 * ds - 1.5 * dS3 / J5 * s;
        //Total derivative of aBar_ij
        const Matrix3d daBar = daBarFirst + daBarSecond + daBarThird;

        //Derivative of a0: da0/dsigma_pq 
        const double da0 = 3.0 * a0 * tan3Alpha * dAlpha;

        // A.8.27: L_ijpq^30 = dc_ij/dsigma_pq
        const Matrix3d L30 = da0 * aBarij + a0 * daBar;

        //A8.25a: L_ijpq^1 = d(dPhi/dI)/dsigma_pq * a_ij + dPhi/dI * L_ijpq^10 = d(dPhi/dI)/dsigma_pq * a_ij 
        const Matrix3d L1 = dPhiI * aij;
        //A8.25a: L_ijpq^2 = d(dPhi/dJ)/dsigma_pq * b_ij + dPhi/dJ * L_ijpq^20  
        const Matrix3d L2 = dPhiJ * bij + dPhi_dJ * L20;
        //A8.25a: L_ijpq^3 = d(dPhi/dalpha)/dsigma_pq * c_ij + dPhi/dalpha * L_ijpq^30 
        const Matrix3d L3 = dPhiAlpha * cij + dPhi_dAlpha * L30;

        storeReducedColumn(drij_dStress, column, L1 + L2 + L3);
    }  
    return drij_dStress;
}

// Limit of A8.25 on the hydrostatic axis (J = 0).
// r_ij = (2I - 3p0) delta_ij + s_ij / N^2 + O(J): the Lode-angle term vanishes with J.
// dr_ij = 2 dI delta_ij + ds_ij / N^2, with N evaluated at alpha = 0.
// Approximate tangent: for Mc != Me the true derivative depends on the direction of approach
// (exact if Mc = Me). r_ij itself is exact, so the CPPM solution is unaffected.
CamClay::Matrix6d CamClay::computeRStressDerivativeHydrostatic(const StressState& state) const
{
    const Matrix3d identity = Matrix3d::Identity();
    Matrix6d drij_dStress = Matrix6d::Zero();
    const double inverseN2 = 1.0 / (state.N * state.N);
    
    for (int column = 0; column < 6; ++column)
    {
        const Matrix3d E_pq = unitStressDirection(column);
        const Matrix3d ds = E_pq  - E_pq.trace() / 3.0 * identity;
        storeReducedColumn(drij_dStress, column, 2.0 * E_pq.trace() * identity + inverseN2 * ds);
    }
    return drij_dStress;
}

// Limit of A8.25 on the triaxial meridians (alpha = +/- pi/6).
// In regularized form, r_ij = (2I - 3p0) delta_ij + s_ij / N^2 - c bracket_ij, c = Bbar / (den N^2), with
// bracket_ij = s_ik s_kj / J - 2/3 J delta_ij - 3/2 (S/J)^3 s_ij.
// On the meridians bracket_ij = 0 and d(sin 3 alpha) = 0 (|sin 3 alpha| = 1 is an extremum),
// so d(den) = 0 and only d(bracket_ij) survives in the Lode-angle term:
// dr_ij = 2 dI delta_ij + ds_ij / N^2 - c d(bracket_ij)
// NOTE: dropping L3 entirely (previous version) omits -c d(bracket_ij) and is wrong when Mc != Me.
CamClay::Matrix6d CamClay::computeRStressDerivativeAlphaBoundary(const StressState& state) const
{
    const Matrix3d identity = Matrix3d::Identity();
    Matrix6d drij_dStress = Matrix6d::Zero();
    const Matrix3d& s = state.stressDev;
    const double J = state.J;
    const Matrix3d sSquared = s * s;
    const double inverseN2 = 1.0 / (state.N * state.N);
    // c = Bbar / (den N^2), Bbar = 3 sqrt(3) (1 - n)
    const double c = Bbar * inverseN2 / state.denominator_g;
    // q3 = (S/J)^3 = +/- 2/(3 sqrt(3)) on the meridians
    const double q3 = state.S3 / (J * J * J);

    for (int column = 0; column < 6; ++column)
    {
        // Unit direction associated with one independent
        // component of the symmetric stress tensor
        const Matrix3d E_pq  = unitStressDirection(column);
        const Matrix3d ds = E_pq  - E_pq.trace() / 3.0 * identity;
        const double dJ = (s.array() * ds.array()).sum() / (2.0 * J);
        //d(bracket_ij) with d(q3) = 0
        const Matrix3d dbracket = (ds * s + s * ds) / J - sSquared * dJ / (J * J) 
            - (2.0 / 3.0) * dJ * identity - 1.5 * q3 * ds;

        storeReducedColumn(drij_dStress, column, 2.0 * E_pq.trace() * identity + inverseN2 * ds - c * dbracket);
    }
    return drij_dStress;
} 

Matrix3d CamClay::unitStressDirection(int column)
{
    Matrix3d E = Matrix3d::Zero();
    if (column == 0) {E(0, 0) = 1.0;}
    else if (column == 1) {E(1, 1) = 1.0;}
    else if (column == 2) {E(2, 2) = 1.0;}
    else if (column == 3) {E(0, 1) = 1.0; E(1, 0) = 1.0;}
    else if (column == 4) {E(1, 2) = 1.0; E(2, 1) = 1.0;}
    else {E(0, 2) = 1.0; E(2, 0) = 1.0;}
    return E;
}

void CamClay::storeReducedColumn(Matrix6d& matrix, int column, const Matrix3d& dr)
{
    matrix(0, column) = dr(0, 0);
    matrix(1, column) = dr(1, 1);
    matrix(2, column) = dr(2, 2);
    matrix(3, column) = 2.0 * dr(0, 1);
    matrix(4, column) = 2.0 * dr(1, 2);
    matrix(5, column) = 2.0 * dr(0, 2);
}

Matrix3d CamClay::reducedColumnToTensor(const Matrix6d& matrix, int column)
{
    Matrix3d tensor;
    tensor(0, 0) = matrix(0, column);
    tensor(1, 1) = matrix(1, column);
    tensor(2, 2) = matrix(2, column);
    tensor(0, 1) = tensor(1, 0) = 0.5 * matrix(3, column);
    tensor(1, 2) = tensor(2, 1) = 0.5 * matrix(4, column);
    tensor(0, 2) = tensor(2, 0) = 0.5 * matrix(5, column);

    return tensor;
}

CamClay::Vector6d CamClay::tensorToVector(const Matrix3d& tensor)
{
    Vector6d vector;
    vector << tensor(0, 0), tensor(1, 1), tensor(2, 2), tensor(0, 1), tensor(1, 2), tensor(0, 2);
    return vector;
}

Matrix3d CamClay::vectorToTensor(const Vector6d& vector)
{
    Matrix3d tensor;
    tensor << vector(0), vector(3), vector(5),
              vector(3), vector(1), vector(4),
              vector(5), vector(4), vector(2);

    return tensor;
}

CamClay::CPPMCoefficients CamClay::computeCPPMCoefficients(const ElasticUpdate& update, const StressState& currentState, const Matrix3d& rij, double p0Old, double DeltaLambda) const
{
    CPPMCoefficients coefficients;
    const Matrix3d identity = Matrix3d::Identity();
    //State at n
    const double pOld = update.pOld;
    //Current flow direction r_ij
    const double rkk = update.rkk;
    //12.55a barx0 
    const double x = update.barX0;
    
    // dGave/dx = Gbar0 pOld d[(e^x - 1)/x]/dx = Gbar0 pOld (x e^x - e^x + 1)/x^2
    // Series near x = 0 to avoid cancellation: 1/2 + x/3 + x^2/8 + x^3/30
    const double dGave_dX0 = Gbar0 * pOld * ((std::abs(x) <= 1.0e-3) 
    ? (0.5 + x / 3.0 + x * x / 8.0 + x * x * x /30.0) 
    : (x * update.expX0 - std::expm1(x)) / (x * x));

    //12.61 Z_ij = 2 DeltaY dGave/dx + pOld e^barX0 delta_ij
    const Matrix3d Zij = 2.0 * dGave_dX0 * update.DeltaY + pOld * update.expX0 * identity;

    //Second derivatives of the surface
    // Reduced gradient convetion used [r11 r22 r33 2r12 2r23 2r13]
    const Matrix6d drij_dstress = computeRStressDerivative(currentState);    
    
    //12.63a: A_ijpq
    for (int column = 0; column < 6; ++column)
    {
        Matrix3d dR_dS = reducedColumnToTensor(drij_dstress, column);
        //dr_kk/dsigma_pq
        const double drkk_dstress = dR_dS.trace();
        //A8.29: dd_ij/dsigma_pq
        const Matrix3d ddij_dstress = dR_dS - drkk_dstress / 3.0 * identity;
        //12.63a: A_ijpq =  delta_ip delta_jq + Kbar0 DeltaLambda Z_ij dr_kk/dsigma_pq + 2 DeltaLambda Gave dd_ij/dsigma_pq
        const Matrix3d Aijpq = unitStressDirection(column) + Kbar0 * DeltaLambda * drkk_dstress * Zij
        + 2.0 * DeltaLambda * update.Gave * ddij_dstress;
        coefficients.A.col(column) = tensorToVector(Aijpq);
    }

    //12.63b: B_ij
    //A8.29: dr_ij/dp0 = -3 delta_ij -> drkk/dp0 = - 9
    const double drkk_dp0 = -9.0;
    ////A8.29f: dd_ij / dp0 = 0;
    //12.63b: B_ij = Kbar0 DeltaLambda Zij drkk/dp0 + 2 DeltaLambda Gave ddij/dp0 = = -9 Kbar0 DeltaLambda  Zij
    coefficients.B = tensorToVector(Kbar0 * DeltaLambda * drkk_dp0 * Zij);

    //12.63c: F_ij = Kbar0 rkk Zij + 2 Gave dij
    coefficients.F = tensorToVector(Kbar0 * rkk * Zij + 2.0 * update.Gave * update.dij);

    // Hardening: p0 = p0^n e^(C1 DeltaLambda r_kk), with r_kk = 6I - 9p0 -> dr_kk/dsigma_pq = 6 delta_pq
    const double hardening_p0 = p0Old * std::exp(C1 * DeltaLambda * rkk);
    //12.63d: H_pq
    //Since rkk = 6 I - 9 p0: drkk/dsigma_pq = 6 delta_pq  
    RowVector6d drkk_dstressReduced;
    drkk_dstressReduced << 6.0, 6.0, 6.0, 0.0, 0.0, 0.0;
    
    //12.63d: H_pq = -C1 DeltaLabda p0 e^(C1 DeltaLambda rkk) dr_kk/dsigma_pq
    coefficients.H = -C1* DeltaLambda * hardening_p0 * drkk_dstressReduced;

    //12.63e: omega = 1 - C1  DeltaLambda  p0 e^(C1 DeltaLambda rkk) dr_kk/dp0
    coefficients.omega = 1.0 - C1 * DeltaLambda * hardening_p0 * drkk_dp0;

    //12.63f: beta = -C1 rkk p0  e^(C1 DeltaLambda rkk)
    coefficients.beta = -C1 * rkk * hardening_p0;

    //12.63g: E_pq = dPhi/dsigma_pq = n_pq = r_pq (shear components doubled: tensorial unknowns sigma_12)
    coefficients.E << rij(0, 0), rij(1, 1), rij(2, 2), 2.0 * rij(0, 1), 2.0 * rij(1, 2), 2.0 * rij(0, 2);

    //12.63h: gamma = dPhi/dp0 = - 3 I
    coefficients.gamma = -3.0 * currentState.I;

    return coefficients;
}

//12.49: DeltaLambda1 = phi^Trial / [n^T C n + K_p]^Trial
double CamClay::computeInitialPlasticMultiplier(const StressState& trialState, double phiTrial, const Matrix3d& rTrial, double p0Old) const
{
    //Trial elastic moduli
    const double pTrial = trialState.I / 3.0;
    const double KTrial = Kbar0 * pTrial;
    const double GTrial = Gbar0 * pTrial;
    const double rkkTrial = rTrial.trace();

    // n^T : C : n = r^T : C : r
    //12.46 (C : r)_ij = K r_kk delta_ij + 2 G (r_ij - 1/3 r_kk delta_ij) = K r_kk delta_ij + 2 G d_ij
    const Matrix3d dTrial = rTrial - rkkTrial / 3.0 * Matrix3d::Identity();
    const Matrix3d Cr = KTrial * rkkTrial * Matrix3d::Identity() + 2.0 * GTrial * dTrial;
    const double rCr = (rTrial.array() * Cr.array()).sum();

    //Plastic modulus Kp
    //The sign is incorrect in 12.23. It should be Kp = + 9 C_1 p p_0 r_kk. 
    const double KpTrial = 9.0 * C1 * pTrial * p0Old * rkkTrial;

    //Initial plastic multiplier 
    // A non-positive denominator (strong softening) gives no useful estimate:
    // start Newton from the elastic trial state instead
    const double denominator = rCr + KpTrial;
    if (!(denominator > 0.0) || !std::isfinite(denominator)) {return 0.0;}

    return phiTrial / denominator;
}

CamClay::CPPMResult CamClay::solveCPPM(const Matrix3d& stressOld, const Matrix3d& de, double p0Old, int subStepLevel) const
{
    //Validation
    if (!(p0Old > 0.0)) {throw std::invalid_argument("Cam-Clay: the preconsolidation pressure must be positive");}
    
    const StressState oldState = computeStressState(stressOld);
    if (!(oldState.I > 0.0)) {throw std::runtime_error("Cam-Clay requires a positive compressive mean pressure");}

    //Step 1: Exact elastic predictor 12.53
    const Matrix3d stressTrial = computeElasticUpdate(oldState, de, Matrix3d::Zero(), 0.0).stress;
    const StressState trialState = computeStressState(stressTrial);
    const double phiTrial = computeYieldFunction(trialState, p0Old);
    
    //Step 2: elastic loading/unloading check
    if (phiTrial <= zeroTolerance * p0Old * p0Old){ return {stressTrial, p0Old, 0.0, false};}

    //Step 3: purely isotropic plastic step (state on the hydrostatic axis, volumetric increment)     
    const double deVol = de.trace();
    const Matrix3d deDev = de - deVol / 3.0 * Matrix3d::Identity();
    const bool isotropicPath  = oldState.J <= isotropicPathTolerance * oldState.I / 3.0
        && deDev.norm() <= isotropicPathTolerance * std::abs(deVol);
    if (isotropicPath ) {return solveIsotropicPlasticStep(oldState, de, p0Old);}

    //Step 4: CPPM Newton-Raphson
    CPPMResult result;
    if (solveReturnMapping(oldState, de, p0Old, trialState, phiTrial, result)) {return result;}

    //Step 5: Newton-Raphson failed. Recursive bisection of the strain increment
    if (subStepLevel >= maxSubStepLevel) {throw std::runtime_error("Cam-Clay CPPM did not converge after the maximum substepping level.");}
    const Matrix3d halfIncrement = 0.5 * de;
    const CPPMResult first = solveCPPM(stressOld, halfIncrement, p0Old, subStepLevel + 1);
    const CPPMResult second = solveCPPM(first.stress, halfIncrement, first.p0, subStepLevel + 1);

    return {second.stress, second.p0, first.DeltaLambda + second.DeltaLambda, first.plastic || second.plastic};
}

bool CamClay::solveReturnMapping(const StressState& oldState, const Matrix3d& de, double p0Old, const StressState& trialState, double phiTrial, CPPMResult& result) const
{
    //Step 1: Initial estimate of DeltaLambda (12.49). Plastic multiplier
    //Plastic trial: Initial Newton estimates
    //Flow direction evaluated at the elastic trial state
    const Matrix3d rijTrial = computeYieldGradient(trialState, p0Old);
    double DeltaLambda = computeInitialPlasticMultiplier(trialState, phiTrial, rijTrial, p0Old);
    //Initial stress estimate
    Matrix3d stress = computeElasticUpdate(oldState, de, rijTrial, DeltaLambda).stress;
    //Initial preconsolidation pressure estimate
    //p0^(1) = p0^n e^(C1 DeltaLamda r_kk^Trial)
    double p0 = p0Old * std::exp(C1 * DeltaLambda * rijTrial.trace());

    //Newton iterations
    for (int iteration = 0; iteration < NLMax; ++iteration)
    {
        //Constitutive equations at the current estimate
        const StressState currentState = computeStressState(stress);
        if (!(currentState.I > 0.0)) {return false;}
        const Matrix3d rij = computeYieldGradient(currentState, p0);
        const ElasticUpdate update = computeElasticUpdate(oldState, de, rij, DeltaLambda);
        const double p0Hardening = p0Old * std::exp(C1 * DeltaLambda * update.rkk);     

        //Residuals 12.59
        const Matrix3d stressResidual = stress - update.stress;           
        const double hardeningResidual = p0 - p0Hardening;
        const double phiResidual = computeYieldFunction(currentState, p0);

        //Convergence check (dimensionless measures)
        const bool converged = stressResidual.norm() <= TolStress * stress.norm()
            && std::abs(hardeningResidual) <= Tolp0 * p0 
            && std::abs(phiResidual) <= TolPhi * p0 * p0;
        if (converged)
        {
            //Negative plastic multiplier is not a physical solution
            if (DeltaLambda < 0.0) {return false;}
            result = {stress, p0, DeltaLambda, true};
            return true;
        }
        
        //Coefficients 12.63a - 12.63h
        //Assemble Newton Jacobian 12.47
        const CPPMCoefficients coefficients = computeCPPMCoefficients(update, currentState, rij, p0Old, DeltaLambda);
        Matrix8d jacobian = Matrix8d::Zero();
        jacobian.block<6, 6>(0, 0) = coefficients.A;
        jacobian.block<6, 1>(0, 6) = coefficients.B;
        jacobian.block<6, 1>(0, 7) = coefficients.F;
        jacobian.block<1, 6>(6, 0) = coefficients.H;
        jacobian(6, 6) = coefficients.omega;
        jacobian(6, 7) = coefficients.beta;
        jacobian.block<1, 6>(7, 0) = coefficients.E;
        jacobian(7, 6) = coefficients.gamma;
        
        //Assemble residual vector
        Vector8d residualVector;
        residualVector << tensorToVector(stressResidual), hardeningResidual, phiResidual;

        // Solve Jacobian * DeltaX = -Residual on the dimensionless system, with p0n as reference stress:
        // rows (residuals):   R_sigma/p0n, R_p0/p0n, phi/p0n^2      ([stress], [stress], [stress^2])
        // columns (unknowns): sigma/p0n, p0/p0n, DeltaLambda*p0n     ([stress], [stress], [1/stress])
        // Every entry of the scaled Jacobian is then dimensionless, so the singularity check
        // and the rounding behaviour do not depend on the stress units
        const Vector8d rowScale = (Vector8d() << Vector6d::Constant(1.0 / p0Old), 1.0 / p0Old, 1.0 / (p0Old * p0Old)).finished();
        const Vector8d columnScale = (Vector8d() << Vector6d::Constant(p0Old), p0Old, 1.0 / p0Old).finished();
        const Matrix8d scaledJacobian = rowScale.asDiagonal() * jacobian * columnScale.asDiagonal();
        const Eigen::FullPivLU<Matrix8d> solver(scaledJacobian);
        if (!solver.isInvertible()) {return false;}
        const Vector8d delta = columnScale.asDiagonal() * solver.solve(-(rowScale.asDiagonal() * residualVector));
        if (!delta.allFinite()) {return false;}

        //Update unknowns
        stress +=vectorToTensor(delta.head<6>());
        p0 += delta(6);
        DeltaLambda += delta(7);

        //Basic physical/numerical checks
        if (!(p0 > 0.0) || !std::isfinite(DeltaLambda)) {return false;}
    }
    return false;
}

// Combining the exact elastic pressure relation p = p^n e^(Kbar0 (deVol - Delta eps_v^p)),
// the exact hardening p0 = p0^n e^(C1 Delta eps_v^p) and the isotropic yield condition p = p0:
// Delta eps_v^p = (ln(p^n/p0^n) + Kbar0 deVol) / (Kbar0 + C1)
CamClay::CPPMResult CamClay::solveIsotropicPlasticStep(const StressState& oldState, const Matrix3d& de, double p0Old) const
{
    const double pOld = oldState.I / 3.0;
    //Delta eps_v^p = DeltaLambda r_kk = (ln(p^n/p0^n) + Kbar0 * deVol) / (Kbar0 + C1)
    //Obtained by combining the exact elastic pressure relation with the exact hardening relation and imposing p^(n+1) = p0^(n+1)
    const double deVolPlastic = (std::log(pOld/p0Old) + Kbar0 * de.trace()) / (Kbar0 + C1);
    //p0^(n+1) = p0^n * e^(C1 DeltaLambda r_kk) = p0^n * e^(C1 Delta eps_v^p)
    const double p0New = p0Old * std::exp(C1 * deVolPlastic);
    //On the nonzero isotropic intersection of the Modified Cam-Clay yield surface: p^(n+1) = p0^(n+1) 
    const double pNew = p0New;
    //At the final isotropic yield state: r_kk = 9 p0^(n+1) 
    const double rkk = 9.0 * p0New;
    const double DeltaLambda = deVolPlastic / rkk;
    return {pNew * Matrix3d::Identity(), p0New, DeltaLambda, true};
}
 
// Initialization
void CamClay::initializeParticle(Particle* particle) const
{
    if (particle == nullptr){throw std::invalid_argument("The material-particle pointer is null.");}
    particle->initializeInternalVariables(index(InternalVariable::count));
    particle->setInternalVariable(index(InternalVariable::p0), initialp0);
    // =====================================================
    // Sign convention
    // MPM-Geomechanics: compression = negative
    // Anandarajah implementation: compression = positive
    // =====================================================
    const StressState state0 = computeStressState(-particle->getStress());
    if (computeYieldFunction(state0, initialp0) > zeroTolerance * initialp0 * initialp0)
    {throw std::invalid_argument("Initial Cam-Clay stress state lies outside the yield surface.");}

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
	const Matrix3d stressOld = -particle->getStress();

    // Strain increment 
	const Matrix3d de = -particle->getStrainIncrement();

    // Preconsolidation pressure at time n from Particle
    const double p0Old = particle->getInternalVariable(index(InternalVariable::p0));

    const CPPMResult result = solveCPPM(stressOld, de, p0Old);

    //Return to MPM sign convention
    particle->setStress(-result.stress);
    particle->setInternalVariable(index(InternalVariable::p0), result.p0);   
}

double CamClay::getSoundSpeed() const
{
    const double KRef = Kbar0 * initialp0;
    const double GRef = Gbar0 * initialp0;
    // sound speed
    return std::sqrt((KRef + 4.0 * GRef / 3.0) / density);
}
