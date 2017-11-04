#ifndef KMATHS_UTILITIES_H
#define KMATHS_UTILITIES_H

#include <cstdlib>	
#include <ctime>
#include "KrawlerLib.h"

namespace Krawler
{
	namespace Maths
	{
		const float PI{ 3.141592f };

		const float PI_OVER_2{ PI / 2.f };

		const float PI_BY_2{ PI  * 2.f };

		template<typename T>
		KRAWLER_API T Degrees(T Theta);

		template<typename T>
		KRAWLER_API	T Radians(T Theta);

		template<typename T>
		KRAWLER_API inline T Clamp(T Min, T Max, T Value);

		template <typename T>
		KRAWLER_API	T Max(T A, T B);

		template <typename T>
		KRAWLER_API T Min(T Value, T Min);

		template<typename T>
		KRAWLER_API T Lerp(T A, T B, float t);

		KRAWLER_API __forceinline void InitRand()
		{
			srand(static_cast<unsigned>(time(0)));
		}

		//Random float between 0 - 1
		KRAWLER_API	__forceinline float RandFloat()
		{
			return (static_cast<float> (rand()) / static_cast<float>(RAND_MAX));
		}

		//Random float between 0 and Max
		KRAWLER_API __forceinline float RandFloat(float Max)
		{
			if (Max != 0)
			{
				return static_cast<float>(rand()) / static_cast<float> (RAND_MAX / Max);
			}
			return(0.f);
		}

		//Random float between max & min
		KRAWLER_API __forceinline float RandFloat(float Min, float Max)
		{
			if (Max - Min != 0)
			{
				return Min + static_cast<float> (rand()) / static_cast<float> (RAND_MAX / (Max - Min));
			}
			return{ 0.f };
		}

		KRAWLER_API __forceinline signed int RandInt(signed int val)
		{
			return rand() % val;
		}
	}

}

#include "KMathsUtilities.inl"

#endif 
