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

double ShapeLinear::computeShape(double xpi, double L, double lp)
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

double ShapeLinear::computeGradient(double xip, double L, double lp)
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

void ShapeLinear::update(Vector3d posParticle, Vector3d posNode, Vector3d cellDim, Vector3d particleSize){

    // relative distance to node
    double rx  = posParticle(0)-posNode(0);
    double ry  = posParticle(1)-posNode(1);
    double rz  = posParticle(2)-posNode(2);

    // update shape functions
    double Sx = computeShape(rx, cellDim(0), 0.0);
    double Sy = computeShape(ry, cellDim(1), 0.0);
    double Sz = computeShape(rz, cellDim(2), 0.0);
	setShape(Sx,Sy,Sz);
    
    // update gradients
    double Gx = computeGradient(rx, cellDim(0), 0.0);
    double Gy = computeGradient(ry, cellDim(1), 0.0);
    double Gz = computeGradient(rz, cellDim(2), 0.0);
    setDerivate(Gx,Gy,Gz);
}