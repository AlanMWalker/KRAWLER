#include "Utilities\KDebug.h"
#include <fstream> //for wofstream
#include <Windows.h>
#include <stdio.h>	//for sprintf
#include <mutex>	

static std::wofstream logFile;
std::mutex logFileMutex;

void Krawler::KPrintf(const wchar_t* szFormat, ...)
{
#ifdef RELEASE_CANDIDATE
	return;
#endif

	wchar_t szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);
	OutputDebugString(szBuff);

	//#ifdef _DEBUG
	wprintf(L"%s", szBuff);
	//#endif 
}

inline std::chrono::high_resolution_clock::time_point Krawler::Profiler::StartFunctionTimer()
{
	return std::chrono::high_resolution_clock::now();
}

inline long long Krawler::Profiler::EndFunctionTimer(const std::chrono::high_resolution_clock::time_point&  t1, const std::wstring& funcName, bool bIsMicroseconds, bool bLogFileInsteadOfConsole)
{
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	long long duration;
	static wchar_t outputString[300];

	if (bIsMicroseconds)
	{
		duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	}
	else
	{
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	}

	swprintf_s(outputString, 300, KTEXT("%s execution time: %lld %s\n"), funcName.c_str(), duration, bIsMicroseconds ? KTEXT("Microseconds") : KTEXT("Milliseconds"));
	if (bLogFileInsteadOfConsole)
	{
		logFileMutex.lock();
		if (!logFile.fail())
		{
			logFile << outputString;
		}
		logFileMutex.unlock();

	}
	else
	{
		KPrintf(KTEXT("%s"), outputString);
	}

	return duration;
}

void Krawler::Profiler::SetupProfiler()
{
	logFile = std::wofstream("profiler_log.txt", std::ios::out);
}

void Krawler::Profiler::ShutdownProfiler()
{
	logFile.close();
}
