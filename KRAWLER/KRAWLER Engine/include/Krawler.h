#ifndef KRAWLER_H
#define KRAWLER_H

#include "KrawlerLib.h"
#include "Utilities\KEngineMacros.h"
#include "Maths\KMaths.hpp"

#include "Utilities\KEngineMacros.h"
#include "Utilities\KDebug.h"

#include <map>
#include <Windows.h>

namespace Krawler
{
	using int8 = signed char;
	using uint8 = unsigned char;

	using int16 = signed short;
	using uint16 = unsigned short;

	using int32 = signed int;
	using uint32 = unsigned int;

	using int64 = signed   __int64;
	using uint64 = unsigned __int64;

	using Vec2f = sf::Vector2f;
	using Vec2i = sf::Vector2i;
	using Vec2u = sf::Vector2u;
	using Vec2d = sf::Vector2u;

	using Point = sf::Vector2f;

	struct KApplicationInitialise;

	enum KInitStatus
	{
		Success = 0,
		Failure = -1,
		Nullptr = -2,
		MissingResource = -3
	};



#define KINIT_CHECK(func)																															\
			Krawler::KInitStatus status = func();																									\
			if(status != Krawler::KInitStatus::Success)																								\
			{																																		\
				std::cout << "Failed to initialise function in: " << __FILE__ << " at line " << __LINE__ << " due to ";								\
				switch(status)																														\
				{																																	\
					case Krawler::KInitStatus::Failure: std::cout << "General Failure!" << std::endl; break;										\
					case Krawler::KInitStatus::Nullptr: std::cout << "Null pointer!" << std::endl; break;											\
					case Krawler::KInitStatus::MissingResource: std::cout << "Missing Resource!" << std::endl; break;								\
				}																																	\
			}



	KRAWLER_API KInitStatus StartupEngine(KApplicationInitialise* windowInit);

	KRAWLER_API void ShutdownEngine();
	KRAWLER_API void RunApplication();

	KRAWLER_API std::wstring GenerateUUID();

};


#endif 
