
#define INITGUID
#include <d3d.h>

#include "../common.h"
#include "../utils.h"

#include "d2gi_ddraw.h"
#include "d2gi_direct3d.h"
#include "d2gi_surface.h"


D2GIDirectDraw::D2GIDirectDraw(IDirectDraw7* pOriginal) : DDrawProxy(pOriginal)
{

}


D2GIDirectDraw::~D2GIDirectDraw()
{

}


HRESULT D2GIDirectDraw::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	if (IsEqualIID(riid, IID_IDirect3D7))
	{
		HRESULT hRes = DDrawProxy::QueryInterface(riid, ppvObj);

		if (SUCCEEDED(hRes))
			*ppvObj = (IDirect3D7*)new D2GIDirect3D((IDirect3D7*)*ppvObj);

		return hRes;
	}

	return DD_FALSE;
}


HRESULT D2GIDirectDraw::CreateSurface(LPDDSURFACEDESC2 lpDesc, LPDIRECTDRAWSURFACE7 FAR* lpSurf , IUnknown FAR* pUnknown)
{
	HRESULT hRes = DDrawProxy::CreateSurface(lpDesc, lpSurf, pUnknown);

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

	return hRes;
}
