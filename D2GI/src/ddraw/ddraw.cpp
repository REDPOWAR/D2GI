
#include "ddraw.h"


DDrawProxy::DDrawProxy(IDirectDraw7* pOriginal) : m_pOriginal(pOriginal)
{

}


DDrawProxy::~DDrawProxy()
{
}


HRESULT DDrawProxy::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	return m_pOriginal->QueryInterface(riid, ppvObj);
}


ULONG DDrawProxy::AddRef()
{
	return m_pOriginal->AddRef();
}


ULONG DDrawProxy::Release()
{
	ULONG uRes = m_pOriginal->Release();

	if (uRes == 0)
		delete this;

	return uRes;
}


HRESULT DDrawProxy::Compact()
{
	return m_pOriginal->Compact();
}


HRESULT DDrawProxy::CreateClipper(DWORD arg1, LPDIRECTDRAWCLIPPER FAR* lpClipper, IUnknown FAR* pUnknown)
{
	return m_pOriginal->CreateClipper(arg1, lpClipper, pUnknown);
}


HRESULT DDrawProxy::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpPalEntry, LPDIRECTDRAWPALETTE FAR* lpPal, IUnknown FAR* pUnknown)
{
	return m_pOriginal->CreatePalette(dwFlags, lpPalEntry, lpPal, pUnknown);
}


HRESULT DDrawProxy::CreateSurface(LPDDSURFACEDESC2 lpSurfDesc, LPDIRECTDRAWSURFACE7 FAR* lpDDS, IUnknown FAR* pUnknown)
{
	return m_pOriginal->CreateSurface(lpSurfDesc, lpDDS, pUnknown);
}


HRESULT DDrawProxy::DuplicateSurface(LPDIRECTDRAWSURFACE7 pSurf, LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return m_pOriginal->DuplicateSurface(pSurf, lpSurf);
}


HRESULT DDrawProxy::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpSurfDesc, LPVOID lpArg, LPDDENUMMODESCALLBACK2 lpCallback)
{
	return m_pOriginal->EnumDisplayModes(dwFlags, lpSurfDesc, lpArg, lpCallback);
}


HRESULT DDrawProxy::EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpSurfDesc, LPVOID lpArg, LPDDENUMSURFACESCALLBACK7 lpCallback)
{
	return m_pOriginal->EnumSurfaces(dwFlags, lpSurfDesc, lpArg, lpCallback);
}


HRESULT DDrawProxy::FlipToGDISurface()
{
	return m_pOriginal->FlipToGDISurface();
}


HRESULT DDrawProxy::GetCaps(LPDDCAPS lpCaps1, LPDDCAPS lpCaps2)
{
	return m_pOriginal->GetCaps(lpCaps1, lpCaps2);
}


HRESULT DDrawProxy::GetDisplayMode(LPDDSURFACEDESC2 lpSurfDesc)
{
	return m_pOriginal->GetDisplayMode(lpSurfDesc);
}


HRESULT DDrawProxy::GetFourCCCodes(LPDWORD lpNums, LPDWORD lpVars)
{
	return m_pOriginal->GetFourCCCodes(lpNums, lpVars);
}


HRESULT DDrawProxy::GetGDISurface(LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return m_pOriginal->GetGDISurface(lpSurf);
}


HRESULT DDrawProxy::GetMonitorFrequency(LPDWORD lpFreq)
{
	return m_pOriginal->GetMonitorFrequency(lpFreq);
}


HRESULT DDrawProxy::GetScanLine(LPDWORD lpOut)
{
	return m_pOriginal->GetScanLine(lpOut);
}


HRESULT DDrawProxy::GetVerticalBlankStatus(LPBOOL lpOut)
{
	return m_pOriginal->GetVerticalBlankStatus(lpOut);
}


HRESULT DDrawProxy::Initialize(GUID FAR* pGUID)
{
	return m_pOriginal->Initialize(pGUID);
}


HRESULT DDrawProxy::RestoreDisplayMode()
{
	return m_pOriginal->RestoreDisplayMode();
}


HRESULT DDrawProxy::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	return m_pOriginal->SetCooperativeLevel(hWnd, dwFlags);
}


HRESULT DDrawProxy::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags)
{
	return m_pOriginal->SetDisplayMode(dwWidth, dwHeight, dwBPP, dwRefreshRate, dwFlags);
}


HRESULT DDrawProxy::WaitForVerticalBlank(DWORD dwFlags, HANDLE handle)
{
	return m_pOriginal->WaitForVerticalBlank(dwFlags, handle);
}


HRESULT DDrawProxy::GetAvailableVidMem(LPDDSCAPS2 lpCaps, LPDWORD lpTotal, LPDWORD lpFree)
{
	return m_pOriginal->GetAvailableVidMem(lpCaps, lpTotal, lpFree);
}


HRESULT DDrawProxy::GetSurfaceFromDC(HDC hDC, LPDIRECTDRAWSURFACE7* lpSurf)
{
	return m_pOriginal->GetSurfaceFromDC(hDC, lpSurf);
}


HRESULT DDrawProxy::RestoreAllSurfaces()
{
	return m_pOriginal->RestoreAllSurfaces();
}


HRESULT DDrawProxy::TestCooperativeLevel()
{
	return m_pOriginal->TestCooperativeLevel();
}


HRESULT DDrawProxy::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 lpDevID, DWORD dwFlags)
{
	return m_pOriginal->GetDeviceIdentifier(lpDevID, dwFlags);
}


HRESULT DDrawProxy::StartModeTest(LPSIZE lpSizes, DWORD dwCount, DWORD dwFlags)
{
	return m_pOriginal->StartModeTest(lpSizes, dwCount, dwFlags);
}


HRESULT DDrawProxy::EvaluateMode(DWORD dwFlags, DWORD* lpOut)
{
	return m_pOriginal->EvaluateMode(dwFlags, lpOut);
}
