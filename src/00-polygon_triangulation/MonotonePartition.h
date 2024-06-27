#pragma once

#include <vector>
#include "Core/Base/Core.h"
#include "Core/Primitives/PolygonDCEL.h"
#include "Core/GeoUtils.h"

namespace jmk {
	void get_monotone_polygons(Polygon2d* poly, std::vector<Polygon2d*>& mono_polies);
}