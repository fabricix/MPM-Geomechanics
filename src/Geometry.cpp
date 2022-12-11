/*
 * Geometry.cpp
 *
 *  Created on: 19 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Geometry.h"

namespace Geometry {

	int pnpoly(int nvert, vector<double> vertx, vector<double> verty, double testx, double testy) {

		// Adaptation of the code by Wm. Randolph Franklin
		// https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
		// Copyright (c) 1970-2003, Wm. Randolph Franklin 

		// nvert: Number of vertices in the polygon.
		// vertx, verty: Arrays containing the x- and y-coordinates of the polygon's vertices.
		// testx, testy: X- and y-coordinate of the test point.

		int i, j, c = 0;
		for (i = 0, j = nvert-1; i < nvert; j = i++) {
			
			if (((verty[i]>testy) != (verty[j]>testy)) && (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]))
				c = !c;
		}

		return c;
	}

	bool getInsidePolygon(const vector<Vector3d>& polygon, const Vector3d& point, string extrude_direction) {

		// create the vectors of horizontal and vertical points 
		// according to the extrude direction
		// o : the axis is out of the plane
		// + : the axis is in to the plane

		vector<double> vec_hor, vec_ver;

		// Extrude direction in y
		//
		//  x
		//  ^
		//  |
		//  o---> z
		//
		if (extrude_direction=="y")
		{
			for (size_t i = 0; i < polygon.size(); ++i)
			{
				vec_hor.push_back(polygon.at(i).z());
				vec_ver.push_back(polygon.at(i).x());
			}

			return pnpoly(static_cast<unsigned int> (polygon.size()), vec_hor, vec_ver, point.z(), point.x());
		}

		// Extrude direction in z
		//
		//  y
		//  ^
		//  |
		//  o---> x
		//
		if (extrude_direction=="z")
		{
			for (size_t i = 0; i < polygon.size(); ++i)
			{
				vec_hor.push_back(polygon.at(i).x());
				vec_ver.push_back(polygon.at(i).y());
			}

			return pnpoly(static_cast<unsigned int>(polygon.size()), vec_hor, vec_ver, point.x(), point.y());
		}

		// Extrude direction in x
		//
		//  z
		//  ^
		//  |
		//  o---> y
		//
		if (extrude_direction=="x")
		{
			for (size_t i = 0; i < polygon.size(); ++i)
			{
				vec_hor.push_back(polygon.at(i).y());
				vec_ver.push_back(polygon.at(i).z());
			}

			return pnpoly(static_cast<unsigned int>(polygon.size()), vec_hor, vec_ver, point.y(), point.z());
		}

		return false;
	}

	void getBoundingBox(const vector<Vector3d>& points, vector<Vector3d>& box) {

		// trial left-bottom point
		Vector3d pmin(1e10, 1e10, 1e10);
		
		// trial right-upper point
		Vector3d pmax(-1e10, -1e10, -1e10);

		// verify bounding box of the points
		for (size_t i = 0; i < points.size(); ++i)
		{   
			// x min
			if (points.at(i).x()<pmin.x()) { pmin.x()=points.at(i).x(); }

			// y min
			if (points.at(i).y()<pmin.y()) { pmin.y()=points.at(i).y(); }

			// z min
			if (points.at(i).z()<pmin.z()) { pmin.z()=points.at(i).z(); }
			
			// x max
			if (points.at(i).x()>pmax.x()) { pmax.x()=points.at(i).x(); }
			
			// y max
			if (points.at(i).y()>pmax.y()) { pmax.y()=points.at(i).y(); }

			// z max
			if (points.at(i).z()>pmax.z()) { pmax.z()=points.at(i).z(); }
		}

		// put the points in the box
		box.push_back(pmin);
		box.push_back(pmax);
	}

	bool getInsideBox(Vector3d p1, Vector3d p2, Vector3d position)
	{
		if (p1.x()<=position.x() && p1.y()<=position.y() && p1.z()<=position.z())
		{
			if (position.x()<=p2.x() && position.y()<=p2.y() && position.z()<=p2.z())
			{
				return true;
			}
		}
		return false;
	}
}
