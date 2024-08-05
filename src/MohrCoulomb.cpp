/*
 * MohrCoulomb.cpp
 *
 *  Created on: 1 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <cmath>

#include <Eigen/Eigenvalues>
using namespace Eigen;

#include <Materials/Material.h>
#include <Materials/MohrCoulomb.h>
#include "Warning.h"

#ifndef PI
#define PI 3.141592653589793
#endif

MohrCoulomb::MohrCoulomb(int id, double density, double young, double poisson, double friction, double cohesion, double dilation, double tensile, MohrCoulomb::Softening softening)
:ElasticJaumann(id, density, young, poisson) {

    // model parameters
    this->friction=friction;
    this->cohesion=cohesion;
    this->tensile = tensile; 
    this->dilation=dilation;
    
    // verify the tensile strength inside the failure criteria
    this->tensile = (this->tensile == 0 && tan(this->friction * PI / 180.0) != 0) ? this->cohesion / tan(this->friction * PI / 180.0) : this->tensile;
    
    // configure softening
    this->softening=softening;
    
    // configure material type
    type=Material::MaterialType::ELASTOPLASTIC;
}

MohrCoulomb::~MohrCoulomb() { }

void MohrCoulomb::updateStress(Particle *particle) const
{
    // update stress as elastic stress
    ElasticJaumann::updateStress(particle);
    
    // get trial elastic stress
    Matrix3d trialStress = particle->getStress();

    // compute principal values and directions
    Eigen::SelfAdjointEigenSolver<Matrix3d> eigensolver(trialStress);

    // principal stresses
    double s1 = eigensolver.eigenvalues()[0];
    double s2 = eigensolver.eigenvalues()[1];
    double s3 = eigensolver.eigenvalues()[2];

    // current parameters of strength
    double cohesion_curr = this->softening.cohesion_softening_active ? this->softening.exponentialSoftening(particle->getPlasticStrain(),this->softening.exponential_shape_factor,this->cohesion,this->softening.cohesion_residual) : this->cohesion;

    double friction_curr = this->softening.friction_softening_active ? this->softening.exponentialSoftening(particle->getPlasticStrain(),this->softening.exponential_shape_factor,this->friction,this->softening.friction_residual) : this->friction;

    double tensile_curr = this->softening.tensile_softening_active ? this->softening.exponentialSoftening(particle->getPlasticStrain(),this->softening.exponential_shape_factor,this->tensile,this->softening.tensile_residual) : this->tensile;
    
    double dilation_curr = this->softening.dilation_softening_active ? this->softening.exponentialSoftening(particle->getPlasticStrain(),this->softening.exponential_shape_factor,this->dilation,this->softening.dilation_residual) : this->dilation;
    
    // model definition variables
    double Nfi  = (1.0+sin(friction_curr*PI/180.0))/(1.0-sin(friction_curr*PI/180.0));
    double Npsi = (1.0+sin(dilation_curr*PI/180.0))/(1.0-sin(dilation_curr*PI/180.0));

    // shear failure criteria
    double fs = s1 - s3*Nfi + 2.0*cohesion_curr*sqrt(Nfi);

    // tensile failure criteria
    double ft = tensile_curr - s3;

    // composite yield criteria
    if (ft<0.0 || fs<0.0)
    {
        // new corrected principal stresses
        double s1N(0), s2N(0), s3N(0);

        // elastic variables to express the Hooke law in term of principal stress
        double a1 = getBulkModulus()+(4.0/3.0*getShearModulus());
        double a2 = getBulkModulus()-(2.0/3.0*getShearModulus());
        
        // straight line dividing shear and tensile failure type
        double ap = sqrt(1.0+(Nfi*Nfi))+Nfi;
        double sp = tensile_curr*Nfi-2.0*cohesion_curr*sqrt(Nfi);
        double hy = s3 - tensile_curr + ap*(s1-sp);
        
        // plastic strain matrix
        Matrix3d dep = Matrix3d::Zero();
        
        // shear failure zone
        if (fs<0.0 && hy<=0.0)  
        {   
            // shear failure correction
            double ls  = fs / ((a1 - a2 * Npsi) - (-a1 * Npsi + a2)*Nfi);
            s1N = s1 - ls*(a1-a2*Npsi);
            s2N = s2 - ls*a2*(1.0-Npsi);
            s3N = s3 - ls*(-a1*Npsi+a2);

            // plastic strain increments
            dep(0,0) = ls;
            dep(1,1) = 0.0;
            dep(2,2) = -ls*Npsi;
        }
        // tensile failure zone
        else if (ft<0.0 && hy>0.0)
        {
            // tensile failure correction
            double lt  = (tensile_curr-s3)/a1;
            s1N = s1 + lt*a2;
            s2N = s2 + lt*a2;
            s3N = s3 + lt*a1;

            // plastic strain increment
            dep(0,0) = 0.0;
            dep(1,1) = 0.0;
            dep(2,2) = -lt;
        }
        else
        {
            assert(0);
        }

        // stress tensor in principal axis
        Matrix3d spal = Matrix3d(Vector3d(s1N,s2N,s3N).asDiagonal());
        
        // eigen vectors
        Matrix3d D = eigensolver.eigenvectors();
        
        // stress tensor in general form
        Matrix3d sres = (D*spal)*D.transpose();

        // sets the new stress
        particle->setStress(sres);

        // update the effective plastic strain
        particle->addPlasticStrain(sqrt(2.0/3.0*(dep*dep.transpose()).trace()));
    }
}
