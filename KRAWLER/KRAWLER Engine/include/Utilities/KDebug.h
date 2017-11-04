#ifndef KDEBUG_H
#define KDEBUG_H

#include <chrono>
#include <string>
#include "KrawlerLib.h"

namespace Krawler
{
	void KPrintf(const wchar_t* szFormat, ...);

	namespace Profiler
	{
		KRAWLER_API inline std::chrono::high_resolution_clock::time_point StartFunctionTimer();

		KRAWLER_API inline long long EndFunctionTimer(const std::chrono::high_resolution_clock::time_point&  t1, std::wstring funcName, bool print = true);


	}
}
#endif
