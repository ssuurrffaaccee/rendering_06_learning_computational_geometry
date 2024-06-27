#pragma once

#include <vector>
#include <iostream>

#include "Core/Primitives/Point.h"
#include "Core/Primitives/Polygon.h"
#include "Core/Primitives/Polyhedron.h"

namespace jmk 
{

	// Compute the points in the convex hull in 3D space using incremental methodology.
	// Assume there are no duplicate points.
	void convexhull3D(std::vector<Point3d>& _points, std::vector<Face*>& faces);

	// Compute the points in the convex hull.
	// TODO : Have not implemented yet
	void convexhull3DQuickhull(std::vector<Point3d>& _points, Polygon& _results);


}