#ifndef KRAWLER_H
#define KRAWLER_H

#include "KPrimitiveTypedefs.h"
#include "Utilities\KEngineMacros.h"

#include "Maths\KMaths.hpp"

namespace Krawler
{
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



	KInitStatus StartupEngine(KApplicationInitialise* windowInit);

	void ShutdownEngine();
	void RunApplication();


};


#endif 
