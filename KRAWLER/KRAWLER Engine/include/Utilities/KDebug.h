#ifndef KDEBUG_H
#define KDEBUG_H
#include <chrono>

namespace Krawler
{
	void KPrintf(const wchar_t* szFormat, ...);
	
	namespace Profiler
	{
		inline std::chrono::high_resolution_clock::time_point StartFunctionTimer()
		{
			return std::chrono::high_resolution_clock::now();
		}
		inline long long EndFunctionTimer(const std::chrono::high_resolution_clock::time_point&  t1, std::string funcName, bool print = true)
		{
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			if (print)
			{
				std::cout << funcName << " execution time: " << duration << " ms" << std::endl;
			}
			return duration;
		}

		
	}
}
#endif
