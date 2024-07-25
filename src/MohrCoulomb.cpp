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
#include "MohrCoulomb.h"

#ifndef PI
#define PI 3.141592653589793
#endif

MohrCoulomb::MohrCoulomb(int id, double density, double young, double poisson, double friction, double cohesion, double dilation, double tensile)
:ElasticJaumann(id, density, young, poisson) {

    this->friction=friction;
    this->cohesion=cohesion;
    this->dilation=dilation;
    this->tensile=tensile;
    type=Material::MaterialType::ELASTOPLASTIC;
}

MohrCoulomb::~MohrCoulomb() { }

double MohrCoulomb::exponentialLaw(double x, double eta, double y_initial, double y_final) const {
    return y_final+(y_initial-y_final)*std::exp(-n*x);
}

void MohrCoulomb::updateStress(Particle* particle) const {

    // update stress as elastic stress
    ElasticJaumann::updateStress(particle);
    
    // get trial elastic stress
    Matrix3d trialStress = particle->getStress();

    // compute principal values and directions
    Eigen::SelfAdjointEigenSolver<Matrix3d> eigensolver(trialStress);

    // principal values
    double s1 = eigensolver.eigenvalues()[0];
    double s2 = eigensolver.eigenvalues()[1];
    double s3 = eigensolver.eigenvalues()[2];

    // model definition variables
    double Nfi  = (1.0+sin(friction*PI/180.0))/(1.0-sin(friction*PI/180.0));
    double Npsi = (1.0+sin(dilation*PI/180.0))/(1.0-sin(dilation*PI/180.0));

    // tensile strengh
    double st = cohesion/tan(friction*PI/180.0);
    
    // verify if the tensile cut-off in inside the failure criteria
    if (this->tensile<st){ st=this->tensile; }

    // shear and tensile failure criteria
    double fs = s1 - s3*Nfi + 2.0*cohesion*sqrt(Nfi);
    double ft = st - s3;

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
        double sp = st*Nfi-2.0*cohesion*sqrt(Nfi);
        double hy = s3 - st + ap*(s1-sp);
        
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
            double lt  = (st-s3)/a1;
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