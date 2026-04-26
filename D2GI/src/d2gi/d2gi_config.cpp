
#include "../common/dir.h"
#include "../common/logger.h"

#include "d2gi_config.h"

#include <algorithm>
#include <shlwapi.h>

WINDOWMODE D2GIConfig::s_eWindowMode    = WINDOWMODE::BORDERLESS;
uint32_t   D2GIConfig::s_dwVideoWidth   = 0, D2GIConfig::s_dwVideoHeight = 0;
uint32_t   D2GIConfig::s_AnisotropyLevel = 1;
uint32_t   D2GIConfig::s_MSAALevel      = 0;
bool       D2GIConfig::s_bEnableVSync   = false;
bool       D2GIConfig::s_bFixAlpha      = true;
wchar_t    D2GIConfig::s_cScreenshotsPath[MAX_PATH];
IMG_FORMAT D2GIConfig::s_eImgFormat     = IMG_FORMAT::IMG_BMP;

uint32_t D2GIConfig::GetVideoWidth()
{
	if (s_dwVideoWidth == 0)
		return GetSystemMetrics(SM_CXSCREEN);

	return s_dwVideoWidth;
}


uint32_t D2GIConfig::GetVideoHeight()
{
	if (s_dwVideoHeight == 0)
		return GetSystemMetrics(SM_CYSCREEN);

	return s_dwVideoHeight;
}

std::basic_string<TCHAR> D2GIConfig::GetConfigFilePath()
{
	TCHAR szConfigFile[MAX_PATH];

	PathCombine(szConfigFile, Directory::GetEXEDirectory(), TEXT("d2gi.ini"));
	return szConfigFile;
}

HookOptions D2GIConfig::ReadFromFile()
{
	HookOptions result;

	const std::basic_string<TCHAR> configFilePath = GetConfigFilePath();
	TCHAR szTempBuf[256];

	GetPrivateProfileString(TEXT("VIDEO"), TEXT("WindowMode"), 
		TEXT("borderless"), szTempBuf, ARRAYSIZE(szTempBuf), configFilePath.c_str());

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

	s_dwVideoWidth   = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("Width"), 0, configFilePath.c_str());
	s_dwVideoHeight  = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("Height"), 0, configFilePath.c_str());
	s_bEnableVSync   = !!GetPrivateProfileInt(TEXT("VIDEO"), TEXT("EnableVSync"), FALSE, configFilePath.c_str());
	result.m_bEnableHooks = !!GetPrivateProfileInt(TEXT("HOOKS"), TEXT("EnableHooks"), result.m_bEnableHooks, configFilePath.c_str());
	result.m_bEnableUIHooks = !!GetPrivateProfileInt(TEXT("HOOKS"), TEXT("EnableUIFix"), result.m_bEnableUIHooks, configFilePath.c_str());
	result.m_bEnableMirrorsHooks = !!GetPrivateProfileInt(TEXT("HOOKS"), TEXT("EnableMirrorsFix"), result.m_bEnableMirrorsHooks, configFilePath.c_str());
	result.m_bEnableAffinityHooks = !!GetPrivateProfileInt(TEXT("HOOKS"), TEXT("AllCoresAffinity"), !result.m_bEnableAffinityHooks, configFilePath.c_str());
	s_bFixAlpha      = !!GetPrivateProfileInt(TEXT("VIDEO"), TEXT("FixAlpha"), TRUE, configFilePath.c_str());

	GetPrivateProfileStringW(L"SCREENSHOTS", L"SavePath", L".\\screenshots",
		s_cScreenshotsPath, MAX_PATH, configFilePath.c_str());

	GetPrivateProfileString(TEXT("SCREENSHOTS"), TEXT("ImageFormat"),
		TEXT("bmp"), szTempBuf, ARRAYSIZE(szTempBuf), configFilePath.c_str());

	if (_tcsicmp(szTempBuf, TEXT("png")) == 0)
		s_eImgFormat = IMG_FORMAT::IMG_PNG;
	else if (_tcsicmp(szTempBuf, TEXT("bmp")) == 0)
		s_eImgFormat = IMG_FORMAT::IMG_BMP;
	else if (_tcsicmp(szTempBuf, TEXT("jpg")) == 0)
		s_eImgFormat = IMG_FORMAT::IMG_JPG;
	else
	{
		Logger::Warning(TEXT("Unknown image format \"%s\", setting it to BMP"), szTempBuf);
		s_eImgFormat = IMG_FORMAT::IMG_BMP;
	}

	const int AnisotropyLevel = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("AnisotropyLevel"), 1, configFilePath.c_str());
	if (AnisotropyLevel > 0) // Protect against people trying to set negative values
	{
		s_AnisotropyLevel = AnisotropyLevel;
	}

	GetPrivateProfileString(TEXT("VIDEO"), TEXT("MSAALevel"),
		TEXT("0"), szTempBuf, ARRAYSIZE(szTempBuf), configFilePath.c_str());
	if (_tcsicmp(szTempBuf, TEXT("max")) == 0)
		s_MSAALevel = 16; // Max supported by D3D9
	else
	{
		const int MSAALevel = GetPrivateProfileInt(TEXT("VIDEO"), TEXT("MSAALevel"), 0, configFilePath.c_str());
		if (MSAALevel >= 2) // Protect against people trying to set negative values or "1x MSAA" (meaningless)
		{
			s_MSAALevel = std::min(MSAALevel, 16);
		}
	}

	return result;
}
