#pragma once


#include <cstdint>
#include <string>

enum class WINDOWMODE
{
	WINDOWED,
	BORDERLESS,
	FULLSCREEN,
};

enum class IMG_FORMAT
{
	IMG_PNG,
	IMG_JPG,
	IMG_BMP,
};

struct HookOptions
{
	bool m_bEnableHooks = true;
	bool m_bEnableUIHooks = false;
	bool m_bEnableMirrorsHooks = true;
	bool m_bEnableAffinityHooks = true;
};

class D2GIConfig
{
	static WINDOWMODE s_eWindowMode;
	static uint32_t   s_dwVideoWidth, s_dwVideoHeight;
	static uint32_t	  s_AnisotropyLevel, s_MSAALevel;
	static bool       s_bEnableVSync;
	static bool       s_bFixAlpha;
	static wchar_t    s_cScreenshotsPath[MAX_PATH];
	static IMG_FORMAT s_eImgFormat;
public:
	static HookOptions ReadFromFile();

	static WINDOWMODE GetWindowMode() { return s_eWindowMode; }
	static uint32_t GetVideoWidth();
	static uint32_t GetVideoHeight();
	static uint32_t AnisotropyLevel() { return s_AnisotropyLevel; }
	static uint32_t MSAALevel() { return s_MSAALevel; }
	static bool VSyncEnabled() { return s_bEnableVSync; }
	static bool FixAlphaEnabled() { return s_bFixAlpha; }
	static wchar_t* GetScreenshotsPath() { return s_cScreenshotsPath; }
	static IMG_FORMAT GetScreenshotsFormat() { return s_eImgFormat; }

	static std::basic_string<TCHAR> GetConfigFilePath();
};
