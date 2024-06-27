#pragma once

#include <vector>
#include <iostream>

#include "Core/Primitives/Point.h"
#include "Core/Primitives/PolygonDCEL.h"
#include "Core/Primitives/Polygon.h"
// #include "MonotonePartition.h"

namespace jmk {
	
	// triangulate the given polygon using ear clipping method
	void triangulate_earclipping( Polygon2dSimple* poly, std::vector<Edge2dSimple>& edge_list);

	// Triangulate the given monotone polygon. Result is undefined if the polygon is not monotone
	void triangulate_monotone(Polygon2d* poly);
}
