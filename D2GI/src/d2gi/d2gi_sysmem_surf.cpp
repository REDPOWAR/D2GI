
#include "d2gi.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_enums.h"


D2GISystemMemorySurface::D2GISystemMemorySurface(D2GI* pD2GI, DWORD dwW, DWORD dwH) 
	: D2GISurface(pD2GI), m_dwWidth(dwW), m_dwHeight(dwH), m_dwCKFlags(0), m_pData(NULL)
{
	LoadResource();
}


D2GISystemMemorySurface::~D2GISystemMemorySurface()
{
	ReleaseResource();
}



HRESULT D2GISystemMemorySurface::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	m_dwCKFlags = dwFlags;
	m_sColorKey = *pCK;

	return DD_OK;
}


VOID D2GISystemMemorySurface::ReleaseResource()
{
	DEL(m_pData);
}


VOID D2GISystemMemorySurface::LoadResource()
{
	m_uDataSize = m_dwWidth * m_dwHeight * m_pD2GI->GetOriginalBPP() / 8;
	m_uPitch = m_dwWidth * m_pD2GI->GetOriginalBPP() / 8;
	m_pData = new BYTE[m_uDataSize];
}


HRESULT D2GISystemMemorySurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD, HANDLE)
{
	if (pRect == NULL)
	{
		ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
		pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
		pDesc->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;

		if (m_dwCKFlags & DDCKEY_SRCBLT)
		{
			pDesc->dwFlags |= DDSD_CKSRCBLT;
			pDesc->ddckCKSrcBlt = m_sColorKey;
		}

		pDesc->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->lPitch = m_uPitch;
		pDesc->ddpfPixelFormat = g_pf16_565;
		pDesc->lpSurface = m_pData;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GISystemMemorySurface::Unlock(LPRECT)
{
	return DD_OK;
}