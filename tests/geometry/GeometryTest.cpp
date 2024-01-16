/*
 * GeometryTest.cpp
 *
 *  Created on: Tue Jul 20 13:58:44 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;

#include "Eigen/Core"
using Eigen::Vector3d;
using Eigen::Vector3i;

#include "Geometry.h"
    
void testInsidePoint(vector<Vector3d> poligon, Vector3d pnt, string dir)
{
    cout<<"the point "<<pnt.transpose()<<" is inside the polygon->"<<Geometry::getInsidePolygon(poligon,pnt,dir)<<"\n";
}

int main(int argc, char **argv)
{
    double tol = 1e-5;

	// create some points
    vector<Vector3d> poligon;
    poligon.push_back(Vector3d(0,0,0)); // p1
    poligon.push_back(Vector3d(2,0,0)); // p2
    poligon.push_back(Vector3d(0,2,0)); // p3

    // plane xy
    //     p3
    //     |\
    //     | \
    //     |__\
    //     p1  p2

    // check if the points are inside the triangle

    testInsidePoint(poligon,Vector3d(-0.1,0,0),"z"); // false

    testInsidePoint(poligon,Vector3d(-0.1,0,-0.1),"z"); // false

    testInsidePoint(poligon,Vector3d(0,0,-0.1),"z"); // false

    testInsidePoint(poligon,Vector3d(0,0,0),"z"); // true

    testInsidePoint(poligon,Vector3d(2,0,0),"z"); // false (the upper bound is out)

    testInsidePoint(poligon,Vector3d(1,0,1),"z"); // false (the upper bound is out)

    testInsidePoint(poligon,Vector3d(0,0,2),"z"); // false (the upper bound is out)

    testInsidePoint(poligon,Vector3d(0,0,2.1),"z"); // false

    testInsidePoint(poligon,Vector3d(1,0,0),"z"); // true

    testInsidePoint(poligon,Vector3d(2-tol,0,0),"z"); // true

    testInsidePoint(poligon,Vector3d(1-tol,1-tol,0),"z"); // true

    testInsidePoint(poligon,Vector3d(0,2-tol,0),"z"); // true

    return 0;
}
