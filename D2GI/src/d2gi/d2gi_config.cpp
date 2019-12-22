
#include "../common/dir.h"
#include "../common/logger.h"

#include "d2gi_config.h"


WINDOWMODE D2GIConfig::s_eWindowMode  = WMODE_BORDERLESS;
DWORD      D2GIConfig::s_dwVideoWidth = 0, D2GIConfig::s_dwVideoHeight = 0;
BOOL       D2GIConfig::s_bEnableHooks = TRUE;
BOOL       D2GIConfig::s_bEnableVSync = FALSE;

DWORD D2GIConfig::GetVideoWidth()
{
	if (s_dwVideoWidth == 0)
		return GetSystemMetrics(SM_CXSCREEN);

	return s_dwVideoWidth;
}


DWORD D2GIConfig::GetVideoHeight()
{
	if (s_dwVideoHeight == 0)
		return GetSystemMetrics(SM_CYSCREEN);

	return s_dwVideoHeight;
}


VOID D2GIConfig::ReadFromFile()
{
	TCHAR szWinMode[256];
	TCHAR szConfigFile[MAX_PATH];

	_tcscpy(szConfigFile, Directory::GetEXEDirectory());
	_tcscat(szConfigFile, TEXT("d2gi.ini"));

	GetPrivateProfileString(TEXT("VIDEO"), TEXT("WindowMode"), 
		TEXT("borderless"), szWinMode, ARRAYSIZE(szWinMode), szConfigFile);

	if (_tcsicmp(szWinMode, TEXT("fullscreen")) == 0)
		s_eWindowMode = WMODE_FULLSCREEN;
	else if (_tcsicmp(szWinMode, TEXT("windowed")) == 0)
		s_eWindowMode = WMODE_WINDOWED;
	else if (_tcsicmp(szWinMode, TEXT("borderless")) == 0)
		s_eWindowMode = WMODE_BORDERLESS;
	else
	{
		Logger::Warning(TEXT("Unknown window mode \"%s\", setting it to borderless"), szWinMode);
		s_eWindowMode = WMODE_BORDERLESS;
	}

	s_dwVideoWidth = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("Width"), 0, szConfigFile);
	s_dwVideoHeight = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("Height"), 0, szConfigFile);
	s_bEnableVSync = !!GetPrivateProfileInt(TEXT("VIDEO"), TEXT("EnableVSync"), TRUE, szConfigFile);
	s_bEnableHooks = !!GetPrivateProfileInt(TEXT("HOOKS"), TEXT("EnableHooks"), TRUE, szConfigFile);
}
