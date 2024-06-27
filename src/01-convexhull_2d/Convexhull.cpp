#include "Convexhull.h"
#include "Core/GeoUtils.h"
#include "Core/Distance.h"
#include "Core/Inclusion.h"

#include <algorithm>
#include <iterator>
#include <list>

using namespace jmk;

void jmk::convexhull2DGiftwrapping(std::vector<Point2d>& _points, std::vector<Point2d>& _convex)
{
	if (_points.size() <= 3)
		return;

	// For giftwarpping algorithm we have to pick a point which should be in the convexhull.
	// This point could be top most, bot most, left most or right most point. I pick bot most

	// Get the bottom point
	Point2d bottom_point = _points[0];

	for (Point2d& point : _points){
		if ((point[Y] < bottom_point[Y])
			|| (point[Y] == bottom_point[Y]) && (point[X] < bottom_point[X]) ){
			bottom_point = point;
		}
	}

	Point2d min_polar_point = _points[0];
	float current_polor_angle = 360.0;

	// Finding the second points in the convexhull by calculating poloar angle related to bottom point
	for (size_t i = 0; i < _points.size(); i++){
		float polar_angle = polarAngle(_points[i], bottom_point);
		if (bottom_point != _points[i] && current_polor_angle > polar_angle){
			current_polor_angle = polar_angle;
			min_polar_point = _points[i];
		}
	}
	
	// Add the first two points of the convexhull
	_convex.push_back(bottom_point);
	_convex.push_back(min_polar_point);
	
	// Make the second point of the current convexhull point list the referece to calculate next point.
	Point2d ref_point = min_polar_point;
	int index_before_last = 0;

	while (true){
		current_polor_angle = 360.0;
		for (size_t i = 0; i < _points.size(); i++){
			Vector2f vec1 = ref_point - _convex[index_before_last]; // vector for line from last two vert in the convexhull
			Vector2f vec2 = _points[i] - ref_point;

			float between_angle = angle(vec1, vec2);
			if ( ref_point != _points[i] && current_polor_angle > between_angle){
				current_polor_angle = between_angle;
				min_polar_point = _points[i];
			}
		}

		if (min_polar_point == bottom_point)
			break;

		index_before_last++;
		_convex.push_back(min_polar_point);
		ref_point = min_polar_point;
	}
}



void jmk::convexhull2DModifiedGrahams(std::vector<Point2d>& _points, 
	std::vector<Point2d>& _convex)
{
	if (_points.size() <= 3)
		return;

	//Sort the points left to right order
	std::sort(_points.begin(), _points.end());

	std::vector<Point2d> l_upper;
	std::vector<Point2d> l_lower;

	// Append left most point and next one l_upper.
	l_upper.push_back(*_points.begin());
	l_upper.push_back(*(std::next(_points.begin())));

	int index = 0;
	for (size_t i = 2; i < _points.size(); i++)
	{
		index = l_upper.size();
		const auto& next_point = _points[i];
		while (l_upper.size() > 1 && left(l_upper[index - 2], l_upper[index - 1], next_point))
		{
			l_upper.pop_back();
			index = l_upper.size();
		}

		l_upper.push_back(next_point);
	}

	//Reverse the points right to left order to construct l_lower
	std::reverse(_points.begin(), _points.end());

	// Append Right most point and next one l_lower.
	l_lower.push_back(*_points.begin());
	l_lower.push_back(*(std::next(_points.begin())));

	for (size_t i = 2; i < _points.size(); i++)
	{
		index = l_lower.size();
		const auto& next_point = _points[i];

		while (l_lower.size() > 1 && left(l_lower[index - 2], l_lower[index - 1], next_point))
		{
			l_lower.pop_back();
			index = l_lower.size();
		}

		l_lower.push_back(next_point);
	}

	// Left-most and Right-most points are repeating. So removed those from one of half hulls
	l_upper.pop_back();
	l_lower.pop_back();

	_convex.insert(_convex.end(), l_upper.begin(), l_upper.end());
	_convex.insert(_convex.end(), l_lower.begin(), l_lower.end());
}

void jmk::convexhull2DIncremental(std::vector<Point3d>& _points, std::vector<Point3d>& _convex)
{
	//Sort the points left to right order
	std::sort(_points.begin(), _points.end(), [](const Point3d& a, const Point3d& b) {
		if ((a[X] < b[X])
			|| (a[X] == b[X]) && (a[Y] < b[Y]))
		{
			return true;
		}
		return false;
	});

	_convex.push_back(_points[0]);
	_convex.push_back(_points[1]);
	_convex.push_back(_points[2]);

	int points_size = _points.size();

	for (size_t i = 3; i < _points.size(); i++)
	{
		// If the points is inside the current hull, we can skip it
		if(isInside(_points[i], _convex))
			continue;

		// Get the index of closetest point in current hull 
		int index = getClosestPointIndex(_points[i], _convex);
		Point3d line_point = _convex[index];
		Point3d current_point;
		int uindex = index + 1;
		while (true)
		{
			current_point = _convex[uindex % points_size];
			
			if (!right(_points[i], line_point, current_point))
				break;

			line_point = current_point;
			uindex += 1;
		}
		
		line_point = _convex[index];
		int lindex = index - 1;
		while (true)
		{
			if (lindex < 0)
				lindex = points_size - 1;

			current_point = _convex[lindex];

			if (!left(_points[i], line_point, current_point))
				break;

			line_point = current_point;
			lindex -= 1;
		}

		std::vector<Point3d> new_convex;

		if (lindex < uindex)
		{
			new_convex.insert(new_convex.begin(), _convex.begin(), _convex.begin() + lindex);
			new_convex.push_back(_convex[lindex]);
			new_convex.insert(new_convex.end(), _convex.begin()+uindex, _convex.end());
		}
		else
		{
			new_convex.insert(new_convex.begin(), _convex.begin() + uindex, _convex.begin()+lindex);
			new_convex.push_back(_convex[lindex]);
		}

		_convex = new_convex;
	}
}

template<typename Iterator>
void getHull(Iterator first, Iterator last, Polygon& _results)
{
	unsigned const length = std::distance(first, last);

	if (length == 1)
	{
		_results.Insert(*first);
	}
	else
	{
		Iterator const mid_point = first + length / 2;
		
		Polygon left_poly;
		Polygon right_poly;

		getHull(first, mid_point, left_poly);
		getHull(mid_point,last, right_poly);

		merge(left_poly, right_poly, _results);
	}
}

void jmk::convexhull2DDivideAndConquer(std::vector<Point3d>& _points, Polygon& _results)
{
	//Sort the points left to right order
	std::sort(_points.begin(), _points.end(), [](const Point3d& a, const Point3d& b) {
		if ((a[X] < b[X])
			|| (a[X] == b[X]) && (a[Y] < b[Y]))
		{
			return true;
		}
		return false;
	});

	getHull(_points.begin(), _points.end(), _results);
}

static void find_hull(std::vector<Point3d>& _points, std::vector<Point3d>& _convex, Point3d& _l, Point3d& _r)
{
	if (!_points.size())
		return;

	if (_points.size() == 1) 
	{
		_convex.push_back(_points[0]);
		return;
	}

	Point3d maxd_point = _points[0];
	float max_d = 0;

	for (Point3d& pnt : _points)
	{
		float this_distance = distance(_l, _r, pnt);
		if (this_distance > max_d)
		{
			max_d = this_distance;
			maxd_point = pnt;
		}
	}

	_convex.push_back(maxd_point);

	std::vector<Point3d> s1;
	std::vector<Point3d> s2;

	for (Point3d& pnt : _points)
	{
		if (leftOrBetween(_l, maxd_point, pnt))
			s1.push_back(pnt);
		else if (left(maxd_point, _r, pnt))
			s2.push_back(pnt);
	}

	find_hull(s1, _convex, _l, _r);
	find_hull(s2, _convex, _r, _l);
}

void jmk::convexhull2DQuickhull(std::vector<Point3d>& _points, std::vector<Point3d>& _convex)
{
	if (_points.size() <= 3)
		return;

	// Step 1 : Get the two extreme points Left top and right bot
	Point3d left_top = _points[0];
	Point3d right_bot = _points[0];

	for (Point3d& point : _points)
	{
		if ((point[X] < left_top[X])
			|| (point[X] == left_top[X]) && (point[Y] > left_top[X]))
		{
			left_top = point;
		}

		if ((point[X] > right_bot[X])
			|| (point[X] == right_bot[X]) && (point[Y] < right_bot[X]))
		{
			right_bot = point;
		}
	}

	_convex.push_back(left_top);
	_convex.push_back(right_bot);

	std::vector<Point3d> s1;
	std::vector<Point3d> s2;

	for (Point3d& point : _points)
	{
		if (leftOrBetween(left_top, right_bot, point))
			s1.push_back(point);
		else if(right(left_top, right_bot, point))
			s2.push_back(point);
	}

	find_hull(s1, _convex, left_top, right_bot);
	find_hull(s2, _convex, right_bot,left_top);
}
