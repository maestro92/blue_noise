#ifndef UTILITY_MATH_H_
#define UTILITY_MATH_H_
#include <sstream> //You'll need to include this header to use 'std::stringstream'.
#include <iostream>
#include <iomanip>
#include "define.h"
#include <vector>

using namespace std;

struct ivec2
{
	int x;
	int y;

	friend ostream& operator<<(ostream& os, const ivec2& vec);
};


struct ivec4
{
	int x;
	int y;
	int z;
	int w;
};


struct vec2
{
	float x;
	float y;
};

namespace utl
{

//	static const float MATH_EPSILON;
	/// utl_Math.cpp


	const float DEGREE_TO_RADIAN = 0.0174;    /// pi/180
	const float RADIAN_TO_DEGREE = 57.32;     /// 180/pi


	string intToStr(int value);
	string floatToStr(float value);

	void setRandSeed(int seed);

	int randInt(int min = 0, int max = 100);
	float randFloat(float min = 0, float max = 1);
	template<class T>
	inline T sqr(const T &x)
	{
		return x * x;
	}
	
	bool sameSign(float a, float b);

	// consider Wrap around
	int GetDistSquared(ivec2 p0, ivec2 p1, int maxW, int maxH, bool checkWrapAround);

	int GetDistSquared(ivec2 p0, ivec2 p1);
}
#endif