/*
 * ShapeLinear.cpp
 *
 *  Created on: 8 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <cstdlib>
using std::abs;

#include "ShapeLinear.h"

ShapeLinear::ShapeLinear():Shape() {

}

ShapeLinear::~ShapeLinear() {
	
}

double ShapeLinear::computeShape(double xpi, double L, double)
{   
    // xpi = relative position of particle respect to the node, xp-xi
    // L = cell dimention
    // lp = half current particle size

    if(abs(xpi)>=L){

        return 0.0;
    }
    
    if(-L<xpi && xpi<=0.0){

        return 1.0+xpi/L;
    }
    
    if(0.0<xpi && xpi<L){

        return 1.0-xpi/L;
    }

    return 0.0;
}

double ShapeLinear::computeGradient(double xip, double L, double)
{   
    // xip = relative position of particle respect to the node, xp-xi
    // L = cell dimension
    // lp = half current particle size

    if(abs(xip)>=L){

        return 0.0;
    }
    
    if(-L<xip && xip<=0.0){

        return 1.0/L;
    }
    
    if(0.0<xip && xip<L){

        return -1.0/L;
    }

    return 0.0;
}

void ShapeLinear::update(const Vector3d& posParticle, const Vector3d& posNode, const Vector3d& cellDim, const Vector3d&){

    // update shape functions
    const double Sx = computeShape(posParticle(0)-posNode(0), cellDim(0), 0.0);
    const double Sy = computeShape(posParticle(1)-posNode(1), cellDim(1), 0.0);
    const double Sz = computeShape(posParticle(2)-posNode(2), cellDim(2), 0.0);
	setShape(Sx,Sy,Sz);
    
    // update gradients
    const double Gx = computeGradient(posParticle(0)-posNode(0), cellDim(0), 0.0);
    const double Gy = computeGradient(posParticle(1)-posNode(1), cellDim(1), 0.0);
    const double Gz = computeGradient(posParticle(2)-posNode(2), cellDim(2), 0.0);
    setDerivate(Gx,Gy,Gz);
}
