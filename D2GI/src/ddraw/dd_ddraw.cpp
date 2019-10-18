
#include "../common.h"

#include "dd_ddraw.h"


DDrawProxy::DDrawProxy() 
{
}


DDrawProxy::~DDrawProxy()
{
}


HRESULT DDrawProxy::Compact()
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::CreateClipper(DWORD arg1, LPDIRECTDRAWCLIPPER FAR* lpClipper, IUnknown FAR* pUnknown)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpPalEntry, LPDIRECTDRAWPALETTE FAR* lpPal, IUnknown FAR* pUnknown)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::CreateSurface(LPDDSURFACEDESC2 lpSurfDesc, LPDIRECTDRAWSURFACE7 FAR* lpDDS, IUnknown FAR* pUnknown)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::DuplicateSurface(LPDIRECTDRAWSURFACE7 pSurf, LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpSurfDesc, LPVOID lpArg, LPDDENUMMODESCALLBACK2 lpCallback)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpSurfDesc, LPVOID lpArg, LPDDENUMSURFACESCALLBACK7 lpCallback)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::FlipToGDISurface()
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetCaps(LPDDCAPS lpCaps1, LPDDCAPS lpCaps2)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetDisplayMode(LPDDSURFACEDESC2 lpSurfDesc)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetFourCCCodes(LPDWORD lpNums, LPDWORD lpVars)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetGDISurface(LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetMonitorFrequency(LPDWORD lpFreq)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetScanLine(LPDWORD lpOut)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetVerticalBlankStatus(LPBOOL lpOut)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::Initialize(GUID FAR* pGUID)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::RestoreDisplayMode()
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::WaitForVerticalBlank(DWORD dwFlags, HANDLE handle)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetAvailableVidMem(LPDDSCAPS2 lpCaps, LPDWORD lpTotal, LPDWORD lpFree)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetSurfaceFromDC(HDC hDC, LPDIRECTDRAWSURFACE7* lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::RestoreAllSurfaces()
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::TestCooperativeLevel()
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 lpDevID, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::StartModeTest(LPSIZE lpSizes, DWORD dwCount, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT DDrawProxy::EvaluateMode(DWORD dwFlags, DWORD* lpOut)
{
	return DDERR_GENERIC;
}
