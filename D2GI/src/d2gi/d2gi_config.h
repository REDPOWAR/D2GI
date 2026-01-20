#pragma once


#include <cstdint>


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
	static uint32_t   s_dwVideoWidth, s_dwVideoHeight;
	static uint32_t	  s_AnisotropyLevel, s_MSAALevel;
	static bool       s_bEnableHooks;
	static bool       s_bEnableVSync;
	static bool       s_bFixAlpha;
	static bool       s_bEnableUIHooks;
	static wchar_t    s_cScreenshotsPath[MAX_PATH];
	static IMG_FORMAT s_eImgFormat;
public:
	static void ReadFromFile();

	static WINDOWMODE GetWindowMode() { return s_eWindowMode; }
	static uint32_t GetVideoWidth();
	static uint32_t GetVideoHeight();
	static uint32_t AnisotropyLevel() { return s_AnisotropyLevel; }
	static uint32_t MSAALevel() { return s_MSAALevel; }
	static bool HooksEnabled() { return s_bEnableHooks; }
	static bool VSyncEnabled() { return s_bEnableVSync; }
	static bool FixAlphaEnabled() { return s_bFixAlpha; }
	static bool UIHooksEnabled() { return s_bEnableUIHooks; }
	static wchar_t* GetScreenshotsPath() { return s_cScreenshotsPath; }
	static IMG_FORMAT GetScreenshotsFormat() { return s_eImgFormat; }
};
