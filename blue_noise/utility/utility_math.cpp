#include "utility_math.h"
#include <assert.h>     /* assert */

// const float utl::MATH_EPSILON = 1e-5;
string utl::intToStr(int value)
{
	stringstream ss;
	ss << value; // add number to string
	return ss.str();
}

string utl::floatToStr(float value)
{
    ostringstream buff;
    buff << std::setprecision(4) << value;
    return buff.str();
}

int utl::randInt(int min, int max)
{
	assert(max > min);
	int diff = max - min;
	int result = rand() & diff;
	return result + min;
}

float utl::randFloat(float min, float max)
{
    float num = (float)rand() / (float) RAND_MAX;
    return min + (max - min) * num;
}

bool utl::sameSign(float a, float b)
{
	if (a > 0 && b > 0)
		return true;
	if (a < 0 && b < 0)
		return true;
	if (a == 0 && b == 0)
		return true;
	return false;
}



