#include <Maths/Maths.h>

int RoundToLowest(float x)
{
	if (x == (int)x) return x;

	if (x >= 0.0f)
		return (int)x;
	else
		return (int)(x - 1);
}

int RoundToHighest(float x)
{
	if (x == (int)x) return x;

	if (x >= 0.0f)
		return (int)x + 1;
	else
		return (int)x;
}