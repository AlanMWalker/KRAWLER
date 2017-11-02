#include "KMathsUtilities.h"

/////////////////////////////////////

template<typename T>
inline T Degrees(T theta)
{
	return{ static_cast <T> (static_cast<double>(theta) * 180.0 / PI) };
}
template<typename T>
inline T Radians(T theta)
{
	return{ static_cast <T> (static_cast<double>(theta) * PI / 180.0 ) };
}

template<typename T>
T Clamp(T Min, T Max, T Value)
{
	if (Value < Min)
		return Min;

	if (Value > Max)
		return Max;

	return (Value);
}

template<typename T>
T Max(T A, T B)
{
	return{ A > B ? A : B };
}

template <typename T>
T Min(T A, T B)
{
	return{ A < B ? A : B };
}
template<typename T>
inline T Lerp(T A, T B, float t)
{
	return (1 - t)*A + t*B;
}
template<typename T>
inline T Lerp()
{
	return T();
}
/////////////////////////////////////
