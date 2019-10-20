
#include "d2gi.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_enums.h"


D2GISystemMemorySurface::D2GISystemMemorySurface(D2GI* pD2GI, DWORD dwW, DWORD dwH) 
	: D2GISurface(pD2GI), m_dwWidth(dwW), m_dwHeight(dwH), m_dwCKFlags(0),
	m_pTexture(NULL), m_pSurface(NULL)
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
	RELEASE(m_pSurface);
	RELEASE(m_pTexture);
}


VOID D2GISystemMemorySurface::LoadResource()
{
	D3D9::D3DFORMAT eTextureFormat;
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	m_dwBPP = m_pD2GI->GetOriginalBPP();

	if (m_dwBPP == 16)
	{
		m_sPixelFormat = g_pf16_565;
		eTextureFormat = D3D9::D3DFMT_A1R5G5B5;
	}
	else if (m_dwBPP == 8)
	{
		m_sPixelFormat = g_pf8_Pal;
		eTextureFormat = D3D9::D3DFMT_A8;
	}

	pDev->CreateTexture(m_dwWidth, m_dwHeight, 1, D3DUSAGE_DYNAMIC, 
		eTextureFormat, D3D9::D3DPOOL_DEFAULT, &m_pTexture, NULL);

	m_pTexture->GetSurfaceLevel(0, &m_pSurface);
}


HRESULT D2GISystemMemorySurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD, HANDLE)
{
	if (pRect == NULL)
	{
		D3D9::D3DLOCKED_RECT sLockedRect;

		m_pSurface->LockRect(&sLockedRect, NULL, D3DLOCK_DISCARD);

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
		pDesc->ddpfPixelFormat = m_sPixelFormat;
		pDesc->lpSurface = sLockedRect.pBits;
		pDesc->lPitch = sLockedRect.Pitch;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GISystemMemorySurface::Unlock(LPRECT)
{
	m_pSurface->UnlockRect();
	return DD_OK;
}



HRESULT D2GISystemMemorySurface::IsLost()
{
	return DD_OK;
}