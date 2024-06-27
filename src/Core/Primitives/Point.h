#pragma once

#include "Core/Base/Vector.h"

namespace jmk {

	typedef Vector2f Point2d;
	typedef Vector3f Point3d;

	static const Point2d DEFAULT_POINT_2D(Limits<float>::MAX, Limits<float>::MAX);
	static const Point3d DEFAULT_POINT_3D(Limits<float>::MAX, Limits<float>::MAX, Limits<float>::MAX);

	bool sort2DLRTB(const Point2d& a, const Point2d& b);

	bool sort3DTBLR(const Point3d& a, const Point3d& b);

	bool sort2DTBLR(const Point2d& a, const Point2d& b);

	bool sort2DbyX(const Point2d& a, const Point2d& b);

	bool sort2DbyY(const Point2d& a, const Point2d& b);

}