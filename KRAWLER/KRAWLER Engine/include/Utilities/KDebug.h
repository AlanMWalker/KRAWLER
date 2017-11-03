#ifndef KDEBUG_H
#define KDEBUG_H

#include <chrono>
#include <Windows.h>
#include <string>

namespace Krawler
{
	void KPrintf(const wchar_t* szFormat, ...);

	namespace Profiler
	{
		inline std::chrono::high_resolution_clock::time_point StartFunctionTimer()
		{
			return std::chrono::high_resolution_clock::now();
		}
		inline long long EndFunctionTimer(const std::chrono::high_resolution_clock::time_point&  t1, std::wstring funcName, bool print = true)
		{
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			if (print)
			{
				//std::cout << funcName << " execution time: " << duration << " ms" << std::endl;
				KPrintf(TEXT("%s execution time: %lld ms\n"), funcName.c_str(), duration);
			}
			return duration;
		}


	}
}
#endif
