#include "Utilities\KDebug.h"
#include <Windows.h>

void Krawler::KPrintf(const wchar_t* szFormat, ...)
{
#ifdef RELEASE_CANDIDATE
	return
#endif

		wchar_t szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);
	OutputDebugString(szBuff);

#ifdef _DEBUG
	wprintf(L"%s", szBuff);
#endif 
}

inline std::chrono::high_resolution_clock::time_point Krawler::Profiler::StartFunctionTimer()
{
	return std::chrono::high_resolution_clock::now();
}

inline long long Krawler::Profiler::EndFunctionTimer(const std::chrono::high_resolution_clock::time_point&  t1, std::wstring funcName, bool print)
{
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	if (print)
	{
		//std::cout << funcName << " execution time: " << duration << " ms" << std::endl;
		KPrintf(L"%s execution time: %lld ms\n", funcName.c_str(), duration);
	}
	return duration;
}
