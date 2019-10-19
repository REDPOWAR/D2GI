
#include "../common.h"
#include "../utils.h"

#define INITGUID
#include "../d3d7.h"
#include "../d3d9.h"

#include "d2gi.h"
#include "d2gi_ddraw.h"
#include "d2gi_direct3d.h"
#include "d2gi_surface.h"
#include "d2gi_enums.h"


D2GIDirectDraw::D2GIDirectDraw(D2GI* pD2GI) : DDrawProxy(), D2GIBase(pD2GI)
{

}


D2GIDirectDraw::~D2GIDirectDraw()
{
	m_pD2GI->OnDirectDrawReleased();
}


HRESULT D2GIDirectDraw::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	/*if (IsEqualIID(riid, IID_IDirect3D7))
	{
		HRESULT hRes = DDrawProxy::QueryInterface(riid, ppvObj);

		if (SUCCEEDED(hRes))
			*ppvObj = (IDirect3D7*)new D2GIDirect3D((IDirect3D7*)*ppvObj);

		return hRes;
	}*/

	return DDERR_GENERIC;
}


HRESULT D2GIDirectDraw::CreateSurface(D3D7::LPDDSURFACEDESC2 lpDesc, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf , IUnknown FAR* pUnknown)
{
	/*HRESULT hRes = DDrawProxy::CreateSurface(lpDesc, lpSurf, pUnknown);

	if (SUCCEEDED(hRes))
	{
		Debug(TEXT("Surface creation (0x%08x):"), *lpSurf);
		DebugSurfaceDesc(lpDesc);
		if ((lpDesc->dwFlags & DDSD_CAPS) && (lpDesc->ddsCaps.dwCaps & DDSCAPS_FLIP))
		{
			DDSURFACEDESC2 sD;
			IDirectDrawSurface7* pAS;

			ZeroMemory(&sD, sizeof(sD));
			sD.dwSize = sizeof(sD);
			sD.dwFlags = DDSD_CAPS;
			Debug(TEXT("This surface is flippable:"));
			(*lpSurf)->GetSurfaceDesc(&sD);
			Debug(TEXT("Root flags: %i"), sD.ddsCaps.dwCaps);
			ZeroMemory(&sD, sizeof(sD));
			sD.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
			(*lpSurf)->GetAttachedSurface(&sD.ddsCaps, &pAS);
			ZeroMemory(&sD, sizeof(sD));
			sD.dwSize = sizeof(sD);
			sD.dwFlags = DDSD_CAPS;
			pAS->GetSurfaceDesc(&sD);
			pAS->Release();
			Debug(TEXT("Back buffer flags: %i"), sD.ddsCaps.dwCaps);

		}
		*lpSurf = (IDirectDrawSurface7*)new D2GISurface((IDirectDrawSurface7*)*lpSurf);
	}

	return hRes;*/

	return DDERR_GENERIC;
}


HRESULT D2GIDirectDraw::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	Debug(TEXT("Setting coop level for window 0x%08x (%i)"), hWnd, dwFlags);

	m_pD2GI->OnCooperativeLevelSet(hWnd, dwFlags);

	return DD_OK;
}


HRESULT D2GIDirectDraw::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags)
{
	Debug(TEXT("Setting display mode %ix%ix%i (%i %i)"), dwWidth, dwHeight, dwBPP, dwRefreshRate, dwFlags);

	m_pD2GI->OnDisplayModeSet(dwWidth, dwHeight, dwBPP, dwFlags);

	return DD_OK;
}


HRESULT D2GIDirectDraw::EnumDisplayModes(DWORD dwFlags, D3D7::LPDDSURFACEDESC2 lpSurfDesc, LPVOID lpArg, D3D7::LPDDENUMMODESCALLBACK2 lpCallback)
{
	INT i;

	for (i = 0; i < (INT)g_uAvailableDisplayModesCount; i++)
	{
		if (!lpCallback(g_asAvailableDisplayModes + i, lpArg))
			break;
	}

	return DD_OK;
}


HRESULT D2GIDirectDraw::GetDeviceIdentifier(D3D7::LPDDDEVICEIDENTIFIER2 lpDID, DWORD)
{
	D3D9::_D3DADAPTER_IDENTIFIER9 sAdapterID;
	D3D9::IDirect3D9* pD3D9 = GetD3D9();

	pD3D9->GetAdapterIdentifier(0, 0, &sAdapterID);
	
	strcpy(lpDID->szDescription, sAdapterID.Description);
	strcpy(lpDID->szDriver, sAdapterID.Driver);
	lpDID->dwDeviceId = sAdapterID.DeviceId;
	lpDID->dwRevision = sAdapterID.Revision;
	lpDID->dwSubSysId = sAdapterID.SubSysId;
	lpDID->dwVendorId = sAdapterID.VendorId;
	lpDID->dwWHQLLevel = sAdapterID.WHQLLevel;
	lpDID->guidDeviceIdentifier = sAdapterID.DeviceIdentifier;
	lpDID->liDriverVersion = sAdapterID.DriverVersion;

	return DD_OK;
}


HRESULT D2GIDirectDraw::GetCaps(D3D7::LPDDCAPS lpHALCaps, D3D7::LPDDCAPS lpHELCaps)
{
	*lpHALCaps = g_sHALCaps;
	*lpHELCaps = g_sHELCaps;

	return DD_OK;
}
