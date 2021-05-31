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

    // compute shape functions
    const double Sx = computeShape(posParticle(0)-posNode(0), cellDim(0), particleSize(0)/2.0);
    const double Sy = computeShape(posParticle(1)-posNode(1), cellDim(1), particleSize(1)/2.0);
    const double Sz = computeShape(posParticle(2)-posNode(2), cellDim(2), particleSize(2)/2.0);
	
    // update shape function
    setShape(Sx,Sy,Sz);
    
    // compute gradients
    const double Gx = computeGradient(posParticle(0)-posNode(0), cellDim(0), particleSize(0)/2.0);
    const double Gy = computeGradient(posParticle(1)-posNode(1), cellDim(1), particleSize(1)/2.0);
    const double Gz = computeGradient(posParticle(2)-posNode(2), cellDim(2), particleSize(2)/2.0);
    
    // set gradients 
    setDerivate(Gx,Gy,Gz);
}