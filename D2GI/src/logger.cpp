
#include "logger.h"
#include "dir.h"


TCHAR Logger::s_szLogPath[MAX_PATH] = {'\0'};
HWND  Logger::s_hWnd = NULL;


VOID Logger::WriteLog(TCHAR* pszMessage)
{
	TCHAR      szFormattedMessage[1024];
	SYSTEMTIME stTime;
	DWORD      dwPID, dwTID;
	FILE*      pFile;

	if (*s_szLogPath == '\0')
	{
		_tcscpy(s_szLogPath, Directory::GetEXEDirectory());
		_tcscat(s_szLogPath, TEXT("d2gi.log"));
	}

	GetSystemTime(&stTime);
	dwPID = GetCurrentProcessId();
	dwTID = GetCurrentThreadId();

	_stprintf(szFormattedMessage, TEXT("[%i-%02i-%02i %02i:%02i:%02i:%03i %i:%i] %s\n"),
		stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour,
		stTime.wMinute, stTime.wSecond, stTime.wMilliseconds, dwPID, dwTID, pszMessage);

#ifdef _DEBUG
	OutputDebugString(szFormattedMessage);
#endif

	pFile = _tfopen(s_szLogPath, TEXT("a"));
	_ftprintf(pFile, TEXT("%s"), szFormattedMessage);
	fclose(pFile);
}


VOID Logger::Error(TCHAR* pszFmt, ...)
{
	TCHAR szMessage[1024];
	va_list args;

	va_start(args, pszFmt);
	_vstprintf(szMessage, pszFmt, args);
	va_end(args);

	WriteLog(szMessage);
	MessageBox(s_hWnd, szMessage, TEXT("D2GI Fatal Error"), MB_OK | MB_ICONERROR);
	ExitProcess(-1);
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
