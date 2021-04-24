/*
 * Shape.cpp
 *
 *  Created on: 22 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Shape.h"
#include "Utilities.h"

#include <cstdlib>
using std::abs;

Shape::Shape() {
	
	Sx=0.0;
	Sy=0.0;
	Sz=0.0;

	Gx=0.0;
	Gy=0.0;
	Gz=0.0;
}

Shape::~Shape() {
	// TODO Auto-generated destructor stub
}

// update the shape funcion values 
double Shape::shapeGimp(double xixp, double L, double lp)
{   
    // dxip = relative position of particle respect to the node, xp-xi
    // lp = half current particle size
    // L = cell dimention

    if (abs(xixp)>=(L+lp))
    {
        return 0.0;
    }

    if ((-L-lp)< xixp && xixp<=(-L+lp))
    {
        return ((L+lp+xixp)*(L+lp+xixp))/(4.0*L*lp);
    }

    if ((-L+lp)<xixp && xixp<=-lp)
    {
        return 1.0+(xixp/L);
    }

    if ((-lp)<xixp && xixp<=lp)
    {
        return 1.0-(xixp*xixp+lp*lp)/(2.0*L*lp);
    }

    if (lp<xixp && xixp<=(L-lp))
    {
        return 1.0-(xixp/L);
    }

    if ((L-lp)<xixp && xixp<=(L+lp))
    {
        return ((L+lp-xixp)*(L+lp-xixp))/(4.0*L*lp);
    }
    return 0.0;
}

// update the shape funcion values 
double Shape::gradientGimp(double xixp, double L, double lp)
{   
    // dxip = relative position of particle respect to the node, xp-xi
    // lp = half current particle size
    // L = cell dimension

    if (abs(xixp)>=(L+lp))
    {
        return 0.0;
    }

    if ((-L-lp)<xixp && xixp<=(-L+lp))
    {
        return (L+lp+xixp)/(2.0*L*lp);
    }

    if ((-L+lp)<xixp && xixp<=-lp)
    {
        return 1.0/L;
    }

    if ((-lp)<xixp && xixp<=lp)
    {
        return (-xixp/(L*lp));
    }

    if (lp<xixp && xixp<=(L-lp))
    {
        return (-1.0/L);
    }

    if ((L-lp)<xixp && xixp<=(L+lp))
    {
        return -(L+lp-xixp)/(2.0*L*lp);
    }

    return 0.0;
}

void Shape::updateGimp(Vector3d posParticle, Vector3d posNode, Vector3d cellDim, Vector3d particleSize){

    // relative distance to node
    double rx  = posParticle(0)-posNode(0);
    double ry  = posParticle(1)-posNode(1);
    double rz  = posParticle(2)-posNode(2);
    
    // lp is half the current particle size (L = 2lp)
    double lpx = particleSize(0)/2.0;
    double lpy = particleSize(1)/2.0;
    double lpz = particleSize(2)/2.0;

	// update particle domain
    #if 0
    lpx = lpx*deformationGradient.xx;
    lpy = lpx*deformationGradient.yy;
    lpz = lpx*deformationGradient.zz;
    #endif

    // update shape functions
    Sx = shapeGimp(rx, cellDim(0), lpx);
    Sy = shapeGimp(ry, cellDim(1), lpy);
    Sz = shapeGimp(rz, cellDim(2), lpz);

    // update gradients
    Gx = gradientGimp(rx, cellDim(0), lpx);
    Gy = gradientGimp(ry, cellDim(1), lpy);
    Gz = gradientGimp(rz, cellDim(2), lpz);
}