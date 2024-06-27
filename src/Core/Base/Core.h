#pragma once
#pragma once

#define _USE_MATH_DEFINES

#include<math.h>
#include <limits>

namespace jmk {
#define TOLERANCE 0.0000000001
#define TOLERANCEL 0.00001
#define TOLERANCELL 0.01
#define ZERO 0.0

#define CW  1
#define CCW 2



	typedef unsigned int uint;
	typedef const unsigned int cuint;
    template<typename T>
    class Limits{
		public:
		static T INF;
		static T MAX;
		static T MIN;
	};
	template<>
	inline float Limits<float>::INF{std::numeric_limits<float>::infinity()};
    template<>
	inline float Limits<float>::MAX{+Limits<float>::INF};
	template<>
	inline float Limits<float>::MIN{+Limits<float>::INF};
	template<>
	inline double Limits<double>::INF{std::numeric_limits<double>::infinity()};
    template<>
	inline double Limits<double>::MAX{+Limits<double>::INF};
	template<>
	inline double Limits<double>::MIN{+Limits<double>::INF};
	enum RELATIVE_POSITION {
		LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION
	};


	enum INTERSECTION_OPS {
		CROSSES,
		POSITIVE,
		NEGATIVE,
		COINCIDENT
	};

	static bool isEqualD(double x, double y)
	{
		return fabs(x - y) < TOLERANCE;
	}

	static bool isEqualDL(double x, double y)
	{
		return fabs(x - y) < TOLERANCEL;
	}

	static bool isEqualDLL(double x, double y)
	{
		return fabs(x - y) < TOLERANCELL;
	}

	static bool _xor (bool x, bool y) {
		return x ^ y;
	}

	static float RadianceToDegrees(float radiance) {
		return radiance * 360 / (2 * M_PI);
	}
}