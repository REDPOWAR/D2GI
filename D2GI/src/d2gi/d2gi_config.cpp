
#include "d2gi_config.h"


WINDOWMODE D2GIConfig::s_eWindowMode = WMODE_BORDERLESS;
DWORD D2GIConfig::s_dwVideoWidth = 0, D2GIConfig::s_dwVideoHeight = 0;


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

}
