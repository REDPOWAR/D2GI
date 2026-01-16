
#include "../common/dir.h"
#include "../common/logger.h"

#include "d2gi_config.h"

#include <shlwapi.h>

WINDOWMODE D2GIConfig::s_eWindowMode    = WINDOWMODE::BORDERLESS;
DWORD      D2GIConfig::s_dwVideoWidth   = 0, D2GIConfig::s_dwVideoHeight = 0;
DWORD      D2GIConfig::s_AnisotropyLevel = 1;
bool       D2GIConfig::s_bEnableHooks   = true;
bool       D2GIConfig::s_bEnableVSync   = false;
bool       D2GIConfig::s_bFixAlpha      = true;
bool       D2GIConfig::s_bEnableUIHooks = true;
wchar_t    D2GIConfig::s_cScreenshotsPath[MAX_PATH];
IMG_FORMAT D2GIConfig::s_eImgFormat     = IMG_BMP;

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
	TCHAR szTempBuf[256];
	TCHAR szConfigFile[MAX_PATH];

	PathCombine(szConfigFile, Directory::GetEXEDirectory(), TEXT("d2gi.ini"));

	GetPrivateProfileString(TEXT("VIDEO"), TEXT("WindowMode"), 
		TEXT("borderless"), szTempBuf, ARRAYSIZE(szTempBuf), szConfigFile);

	if (_tcsicmp(szTempBuf, TEXT("fullscreen")) == 0)
		s_eWindowMode = WINDOWMODE::FULLSCREEN;
	else if (_tcsicmp(szTempBuf, TEXT("windowed")) == 0)
		s_eWindowMode = WINDOWMODE::WINDOWED;
	else if (_tcsicmp(szTempBuf, TEXT("borderless")) == 0)
		s_eWindowMode = WINDOWMODE::BORDERLESS;
	else
	{
		Logger::Warning(TEXT("Unknown window mode \"%s\", setting it to borderless"), szTempBuf);
		s_eWindowMode = WINDOWMODE::BORDERLESS;
	}

	s_dwVideoWidth   = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("Width"), 0, szConfigFile);
	s_dwVideoHeight  = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("Height"), 0, szConfigFile);
	s_bEnableVSync   = !!GetPrivateProfileInt(TEXT("VIDEO"), TEXT("EnableVSync"), FALSE, szConfigFile);
	s_bEnableHooks   = !!GetPrivateProfileInt(TEXT("HOOKS"), TEXT("EnableHooks"), TRUE, szConfigFile);
	s_bEnableUIHooks = !!GetPrivateProfileInt(TEXT("HOOKS"), TEXT("EnableUIFix"), TRUE, szConfigFile);
	s_bFixAlpha      = !!GetPrivateProfileInt(TEXT("VIDEO"), TEXT("FixAlpha"), TRUE, szConfigFile);

	GetPrivateProfileStringW(L"SCREENSHOTS", L"SavePath", L".\\screenshots",
		s_cScreenshotsPath, MAX_PATH, szConfigFile);

	GetPrivateProfileString(TEXT("SCREENSHOTS"), TEXT("ImageFormat"),
		TEXT("bmp"), szTempBuf, ARRAYSIZE(szTempBuf), szConfigFile);

	if (_tcsicmp(szTempBuf, TEXT("png")) == 0)
		s_eImgFormat = IMG_PNG;
	else if (_tcsicmp(szTempBuf, TEXT("bmp")) == 0)
		s_eImgFormat = IMG_BMP;
	else if (_tcsicmp(szTempBuf, TEXT("jpg")) == 0)
		s_eImgFormat = IMG_JPG;
	else
	{
		Logger::Warning(TEXT("Unknown image format \"%s\", setting it to BMP"), szTempBuf);
		s_eImgFormat = IMG_BMP;
	}

	const INT AnisotropyLevel = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("AnisotropyLevel"), 1, szConfigFile);
	if (AnisotropyLevel > 0) // Protect against people trying to set negative values
	{
		s_AnisotropyLevel = AnisotropyLevel;
	}
}
