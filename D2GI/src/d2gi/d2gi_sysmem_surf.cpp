
#include "../logger.h"

#include "d2gi.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_enums.h"
#include "d2gi_palette.h"


D2GISystemMemorySurface::D2GISystemMemorySurface(D2GI* pD2GI, 
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GITexture(pD2GI, dwWidth, dwHeight, eFormat, 1)
{
}


D2GISystemMemorySurface::~D2GISystemMemorySurface()
{
}


HRESULT D2GISystemMemorySurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE h)
{
	HRESULT hRes = D2GITexture::Lock(pRect, pDesc, dwFlags, h);

	if (SUCCEEDED(hRes))
	{
		pDesc->dwFlags ^= DDSD_MIPMAPCOUNT;
		pDesc->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
	}
	
	return hRes;
}


HRESULT D2GISystemMemorySurface::GetCaps(D3D7::LPDDSCAPS2 pCaps)
{
	ZeroMemory(pCaps, sizeof(D3D7::DDSCAPS2));
	pCaps->dwCaps = DDSCAPS_SYSTEMMEMORY;

	return DD_OK;
}
