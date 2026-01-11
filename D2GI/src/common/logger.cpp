
#include "logger.h"
#include "dir.h"

#include <shlwapi.h>

TCHAR Logger::s_szLogPath[MAX_PATH] = {'\0'};
HWND  Logger::s_hWnd = NULL;


VOID Logger::WriteLog(TCHAR* pszMessage)
{
	TCHAR      szFormattedMessage[1024];
	SYSTEMTIME stTime;

	if (*s_szLogPath == '\0')
	{
		PathCombine(s_szLogPath, Directory::GetEXEDirectory(), TEXT("d2gi.log"));
	}

	GetLocalTime(&stTime);
	const DWORD dwPID = GetCurrentProcessId();
	const DWORD dwTID = GetCurrentThreadId();

	_stprintf_s(szFormattedMessage, TEXT("[%u-%02u-%02u %02u:%02u:%02u:%03u %u:%u] %s\n"),
		stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour,
		stTime.wMinute, stTime.wSecond, stTime.wMilliseconds, dwPID, dwTID, pszMessage);

#ifdef _DEBUG
	OutputDebugString(szFormattedMessage);
#endif

	FILE* pFile = _tfopen(s_szLogPath, TEXT("a"));
	if (pFile != nullptr)
	{
		_fputts(szFormattedMessage, pFile);
		fclose(pFile);
	}
}


VOID Logger::Error(TCHAR* pszFmt, ...)
{
	TCHAR szMessage[1024];
	TCHAR* pszMsgWithoutPrefix;
	va_list args;

	va_start(args, pszFmt);
	_tcscpy(szMessage, TEXT("***FATAL ERROR***\n"));
	pszMsgWithoutPrefix = szMessage + _tcslen(szMessage);
	_vstprintf(pszMsgWithoutPrefix, pszFmt, args);
	va_end(args);

	WriteLog(szMessage);
	MessageBox(s_hWnd, pszMsgWithoutPrefix, TEXT("D2GI Fatal Error"), MB_OK | MB_ICONERROR);
	ExitProcess(-1);
}


VOID Logger::Warning(TCHAR* pszFmt, ...)
{
	TCHAR szMessage[1024];
	va_list args;

	va_start(args, pszFmt);
	_tcscpy(szMessage, TEXT("!!!WARNING!!! "));
	_vstprintf(szMessage + _tcslen(szMessage), pszFmt, args);
	va_end(args);

	WriteLog(szMessage);
}


VOID Logger::Log(TCHAR* pszFmt, ...)
{

	TCHAR szMessage[1024];
	va_list args;

	va_start(args, pszFmt);
	_vstprintf(szMessage, pszFmt, args);
	va_end(args);

	WriteLog(szMessage);
}
