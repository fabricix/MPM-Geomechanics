/*
 * ShapeGimp.cpp
 *
 *  Created on: 8 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <cstdlib>
using std::abs;

#include "ShapeGimp.h"

ShapeGimp::ShapeGimp():Shape(){
	
}

ShapeGimp::~ShapeGimp() {

}

double ShapeGimp::computeShape(double xpi, double L, double lp)
{   
    // xpi = relative position of particle respect to the node, xp-xi
    // L = cell dimention
    // lp = half current particle size

    if (abs(xpi)>=(L+lp))
    {
        return 0.0;
    }

    if ((-L-lp)< xpi && xpi<=(-L+lp))
    {
        return ((L+lp+xpi)*(L+lp+xpi))/(4.0*L*lp);
    }

    if ((-L+lp)<xpi && xpi<=-lp)
    {
        return 1.0+(xpi/L);
    }

    if ((-lp)<xpi && xpi<=lp)
    {
        return 1.0-(xpi*xpi+lp*lp)/(2.0*L*lp);
    }

    if (lp<xpi && xpi<=(L-lp))
    {
        return 1.0-(xpi/L);
    }

    if ((L-lp)<xpi && xpi<=(L+lp))
    {
        return ((L+lp-xpi)*(L+lp-xpi))/(4.0*L*lp);
    }
    return 0.0;
}

double ShapeGimp::computeGradient(double xip, double L, double lp)
{   
    // xip = relative position of particle respect to the node, xp-xi
    // L = cell dimension
    // lp = half current particle size

    if (abs(xip)>=(L+lp))
    {
        return 0.0;
    }

    if ((-L-lp)<xip && xip<=(-L+lp))
    {
        return (L+lp+xip)/(2.0*L*lp);
    }

    if ((-L+lp)<xip && xip<=-lp)
    {
        return 1.0/L;
    }

    if ((-lp)<xip && xip<=lp)
    {
        return (-xip/(L*lp));
    }

    if (lp<xip && xip<=(L-lp))
    {
        return (-1.0/L);
    }

    if ((L-lp)<xip && xip<=(L+lp))
    {
        return -(L+lp-xip)/(2.0*L*lp);
    }

    return 0.0;
}

void ShapeGimp::update(const Vector3d& posParticle, const Vector3d& posNode, const Vector3d& cellDim, const Vector3d& particleSize){

    // relative distance to node
    const double rx  = posParticle(0)-posNode(0);
    const double ry  = posParticle(1)-posNode(1);
    const double rz  = posParticle(2)-posNode(2);
    
    // lp is half the current particle size (L = 2lp)
    const double lpx = particleSize(0)/2.0;
    const double lpy = particleSize(1)/2.0;
    const double lpz = particleSize(2)/2.0;

	// update particle domain
    #if 0
    lpx = lpx*deformationGradient.xx;
    lpy = lpx*deformationGradient.yy;
    lpz = lpx*deformationGradient.zz;
    #endif

    // update shape functions
    const double Sx = computeShape(rx, cellDim(0), lpx);
    const double Sy = computeShape(ry, cellDim(1), lpy);
    const double Sz = computeShape(rz, cellDim(2), lpz);
	
    setShape(Sx,Sy,Sz);
    
    // update gradients
    const double Gx = computeGradient(rx, cellDim(0), lpx);
    const double Gy = computeGradient(ry, cellDim(1), lpy);
    const double Gz = computeGradient(rz, cellDim(2), lpz);
    
    setDerivate(Gx,Gy,Gz);
}
