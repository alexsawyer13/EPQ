#include <Maths/Maths.h>

int RoundToLowest(float x)
{
	if (x == (int)x) return x;

	if (x >= 0)
		return (int)x;
	else
		return (int)(x - 1);
}