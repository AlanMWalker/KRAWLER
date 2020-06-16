#ifndef KMATHS_UTILITIES_H
#define KMATHS_UTILITIES_H

#include <cstdlib>	
#include <ctime>
#include "KrawlerLib.h"

namespace Krawler
{
	namespace Maths
	{
		constexpr float PI{ 3.141592f };

		constexpr float PI_OVER_2{ PI / 2.f };

		constexpr float PI_BY_2{ PI  * 2.f };

		template<typename T>
		T Degrees(T Theta);

		template<typename T>
		T Radians(T Theta);

		template<typename T>
		inline T Clamp(T Min, T Max, T Value);

		template <typename T>
		T Max(T A, T B);

		template <typename T>
		T Min(T Value, T Min);

		template<typename T>
		T Lerp(T A, T B, float t);

		KRAWLER_API FORCE_INLINE void InitRand();

		//Random float between 0 - 1
		KRAWLER_API	FORCE_INLINE float RandFloat();
		
		//Random float between 0 and Max
		KRAWLER_API FORCE_INLINE float RandFloat(float Max);

		//Random float between max & min
		KRAWLER_API FORCE_INLINE float RandFloat(float Min, float Max);

		KRAWLER_API FORCE_INLINE signed int RandInt(signed int val);
	}

}

#include "KMathsUtilities.inl"

#endif 
