#ifndef KDEBUG_H
#define KDEBUG_H

#include <chrono>
#include <string>
#include "KrawlerLib.h"
#include "KEngineMacros.h"


namespace Krawler
{
	KRAWLER_API void KPrintf(const wchar_t* szFormat, ...);
#define KPRINTF(text) KPrintf(KTEXT(text))
#define KPRINTF_A(text, args) KPrintf(KTEXT(text), args)

	namespace Profiler
	{
		KRAWLER_API inline std::chrono::high_resolution_clock::time_point StartFunctionTimer();

		KRAWLER_API inline long long EndFunctionTimer(const std::chrono::high_resolution_clock::time_point&  t1, std::wstring funcName, bool bIsMicroSeconds = true, bool print = true);


	}
}
#endif
