#ifndef KRAWLER_H
#define KRAWLER_H

#include "KPrimitiveTypedefs.h"

namespace Krawler
{
	enum KInitStatus
	{
		Success = 0,
		Failure = -1,
		Nullptr = -2
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
				}																																	\
			}


	struct KWindowInitialise
	{
		uint32 width; // Width of the window 
		uint32 height; // Height of the window
		std::string windowTitle; //title of the window
		bool consoleWindow; // Is the console window enabled on your build
	};

	KInitStatus InitialiseEngine(const KWindowInitialise& windowInit);

};


#endif 
