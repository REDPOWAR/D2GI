#pragma once


#include "common.h"


class Logger
{
	static TCHAR s_szLogPath[MAX_PATH];
	static HWND  s_hWnd;

	static VOID WriteLog(TCHAR*);
public:
	static VOID SetHWND(HWND hWnd) { s_hWnd = hWnd; };
	static VOID Error(TCHAR*, ...);
	static VOID Log(TCHAR*, ...);
};
