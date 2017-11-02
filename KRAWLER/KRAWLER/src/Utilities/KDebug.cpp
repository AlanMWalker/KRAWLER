#include "stdafx.h"
#include "Utilities\KDebug.h"

void Krawler::KPrintf(const wchar_t* szFormat, ...)
{
#ifndef _DEBUG
	return;
#endif 
	wchar_t szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);
	OutputDebugString(szBuff);
}
