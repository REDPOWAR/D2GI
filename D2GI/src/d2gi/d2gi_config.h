#pragma once


#include "d2gi_common.h"


enum class WINDOWMODE
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
};

enum IMG_FORMAT
{
	IMG_PNG,
	IMG_JPG,
	IMG_BMP,
};

class D2GIConfig
{
	static WINDOWMODE s_eWindowMode;
	static DWORD      s_dwVideoWidth, s_dwVideoHeight;
	static DWORD	  s_AnisotropyLevel;
	static bool       s_bEnableHooks;
	static bool       s_bEnableVSync;
	static bool       s_bFixAlpha;
	static bool       s_bEnableUIHooks;
	static wchar_t    s_cScreenshotsPath[MAX_PATH];
	static IMG_FORMAT s_eImgFormat;
public:
	static VOID ReadFromFile();

	static WINDOWMODE GetWindowMode() { return s_eWindowMode; }
	static DWORD GetVideoWidth();
	static DWORD GetVideoHeight();
	static DWORD AnisotropyLevel() { return s_AnisotropyLevel; }
	static bool HooksEnabled() { return s_bEnableHooks; }
	static bool VSyncEnabled() { return s_bEnableVSync; }
	static bool FixAlphaEnabled() { return s_bFixAlpha; }
	static bool UIHooksEnabled() { return s_bEnableUIHooks; }
	static wchar_t* GetScreenshotsPath() { return s_cScreenshotsPath; }
	static IMG_FORMAT GetScreenshotsFormat() { return s_eImgFormat; }
};
