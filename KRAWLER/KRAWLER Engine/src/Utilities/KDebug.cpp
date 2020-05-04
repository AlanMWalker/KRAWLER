#include "Utilities\KDebug.h"
#include <fstream> //for wofstream
#include <Windows.h>
#include <stdio.h>	//for sprintf
#include <mutex>	
#include <sstream>
#include <iomanip>
#include <SFML/System/String.hpp>
#include <chrono>

static std::wofstream profilerLog;
static std::wofstream outputLog;

static std::mutex profilerLogMutex;
static std::mutex outputLogFileMutex;

void Krawler::KPrintf(const wchar_t* szFormat, ...)
{
#ifdef RELEASE_CANDIDATE
	return;
#endif
	static char buffer[256];
	static char timeBuffer[256];
	static wchar_t dt[100];

	std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	_wctime_s(dt, 100 * sizeof(char), &tt);
	std::wstring timestamp(dt);
	timestamp = std::wstring(timestamp.begin(), timestamp.end() - 1);
	timestamp.append(L" | ");

	std::lock_guard<std::mutex> lock(outputLogFileMutex);
	wchar_t szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	OutputDebugString(timestamp.c_str());
	_vsnwprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);
	OutputDebugString(szBuff);

	//#ifdef _DEBUG
	wprintf(L"%s", szBuff);

	outputLog << timestamp << szBuff;

	//#endif 
}

inline std::chrono::high_resolution_clock::time_point Krawler::Profiler::StartFunctionTimer()
{
	return std::chrono::high_resolution_clock::now();
}

inline long long Krawler::Profiler::EndFunctionTimer(const std::chrono::high_resolution_clock::time_point& t1, const std::wstring& funcName, bool bIsMicroseconds, bool bLogFileInsteadOfConsole)
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
		profilerLogMutex.lock();
		if (!profilerLog.fail())
		{
			profilerLog << outputString;
		}
		profilerLogMutex.unlock();

	}
	else
	{
		KPrintf(KTEXT("%s"), outputString);
	}

	return duration;
}

void Krawler::Profiler::SetupProfiler()
{
	profilerLog = std::wofstream("profiler_log.txt", std::ios::out);
	outputLog = std::wofstream("output.txt", std::ios::out);
}

void Krawler::Profiler::ShutdownProfiler()
{
	profilerLog.close();
	outputLog.close();
}
