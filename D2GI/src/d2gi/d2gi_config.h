#pragma once


#include "d2gi_common.h"


enum WINDOWMODE
{
	WMODE_WINDOWED,
	WMODE_BORDERLESS,
	WMODE_FULLSCREEN,
};


class D2GIConfig
{
	static WINDOWMODE s_eWindowMode;
	static DWORD      s_dwVideoWidth, s_dwVideoHeight;
public:
	static VOID ReadFromFile();

	static WINDOWMODE GetWindowMode() { return s_eWindowMode; };
	static DWORD GetVideoWidth();
	static DWORD GetVideoHeight();
};
