#ifndef UTILITY_MATH_H_
#define UTILITY_MATH_H_
#include <sstream> //You'll need to include this header to use 'std::stringstream'.
#include <iostream>
#include <iomanip>
#include "define.h"
#include <vector>

using namespace std;

namespace utl
{

//	static const float MATH_EPSILON;
	/// utl_Math.cpp


	const float DEGREE_TO_RADIAN = 0.0174;    /// pi/180
	const float RADIAN_TO_DEGREE = 57.32;     /// 180/pi


	string intToStr(int value);
	string floatToStr(float value);

	int randInt(int min = 0, int max = 100);
	float randFloat(float min = 0, float max = 1);
	template<class T>
	inline T sqr(const T &x)
	{
		return x * x;
	}
	
	bool sameSign(float a, float b);
}
#endif