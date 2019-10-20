
#define INITGUID
#include "../common.h"
#include "../utils.h"

#include "../d3d7.h"
#include "../d3d9.h"

#include "d2gi.h"
#include "d2gi_ddraw.h"
#include "d2gi_direct3d.h"
#include "d2gi_enums.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_sysmem_surf.h"


D2GIDirectDraw::D2GIDirectDraw(D2GI* pD2GI) : DDrawProxy(), D2GIBase(pD2GI)
{
	m_pSurfaceContainer = new D2GIResourceContainer(m_pD2GI);
}


D2GIDirectDraw::~D2GIDirectDraw()
{
	DEL(m_pSurfaceContainer);
	m_pD2GI->OnDirectDrawReleased();
}


HRESULT D2GIDirectDraw::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	if (IsEqualIID(riid, D3D7::IID_IDirect3D7))
	{
		*ppvObj = (D3D7::IDirect3D7*)new D2GIDirect3D(m_pD2GI);

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GIDirectDraw::CreateSurface(D3D7::LPDDSURFACEDESC2 lpDesc, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf , IUnknown FAR* pUnknown)
{
	if ((lpDesc->dwFlags & DDSD_CAPS) && (lpDesc->ddsCaps.dwCaps & DDSCAPS_FLIP))
	{
		D2GIPrimaryFlippableSurface* pSurf = new D2GIPrimaryFlippableSurface(m_pD2GI);

		m_pSurfaceContainer->Add((D2GIResource*)pSurf);
		*lpSurf = (D3D7::IDirectDrawSurface7*)pSurf;
		
		return DD_OK;
	}

	if ((lpDesc->dwFlags & DDSD_CAPS) && (lpDesc->dwFlags & DDSD_WIDTH)
		&& (lpDesc->dwFlags & DDSD_HEIGHT) && (lpDesc->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY))
	{
		D2GISystemMemorySurface* pSurf = new D2GISystemMemorySurface(m_pD2GI, lpDesc->dwWidth, lpDesc->dwHeight);

		m_pSurfaceContainer->Add((D2GIResource*)pSurf);
		*lpSurf = (D3D7::IDirectDrawSurface7*)pSurf;

		return DD_OK;
	}

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


VOID D2GIDirectDraw::ReleaseResources()
{
	m_pSurfaceContainer->ReleaseResources();
}


VOID D2GIDirectDraw::LoadResources()
{
	m_pSurfaceContainer->LoadResources();
}


HRESULT D2GIDirectDraw::GetAvailableVidMem(D3D7::LPDDSCAPS2 pCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree)
{
	*lpdwTotal = *lpdwFree = 0;

	/*if (pCaps->dwCaps & DDSCAPS_TEXTURE)
	{
		*lpdwFree = *lpdwTotal = 1778384896;
		return DD_OK;
	}*/

	return DDERR_GENERIC;
}
