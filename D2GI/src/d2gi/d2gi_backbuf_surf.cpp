
#include "d2gi_backbuf_surf.h"
#include "d2gi.h"
#include "d2gi_enums.h"


D2GIBackBufferSurface::D2GIBackBufferSurface(D2GI* pD2GI) 
	: D2GISurface(pD2GI), m_pTexture(NULL), m_pSurface(NULL)
{
	LoadResource();
}


D2GIBackBufferSurface::~D2GIBackBufferSurface()
{
	ReleaseResource();
}


VOID D2GIBackBufferSurface::ReleaseResource()
{
	RELEASE(m_pSurface);
	RELEASE(m_pTexture);
}


VOID D2GIBackBufferSurface::LoadResource()
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	m_dwWidth = m_pD2GI->GetOriginalWidth();
	m_dwHeight = m_pD2GI->GetOriginalHeight();
	m_dwBPP = m_pD2GI->GetOriginalBPP();

	if (m_dwBPP == 16)
	{
		pDev->CreateTexture(m_dwWidth, m_dwHeight, 1, D3DUSAGE_DYNAMIC, 
			D3D9::D3DFMT_A1R5G5B5, D3D9::D3DPOOL_DEFAULT, &m_pTexture, NULL);

		m_pTexture->GetSurfaceLevel(0, &m_pSurface);
		m_sPixelFormat = g_pf16_565;
	}
}


HRESULT D2GIBackBufferSurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE)
{
	if (pRect == NULL)
	{
		D3D9::D3DLOCKED_RECT sLockedRect;

		m_pSurface->LockRect(&sLockedRect, NULL, D3DLOCK_DISCARD);
		
		ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
		pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
		pDesc->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;
		pDesc->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = m_sPixelFormat;
		pDesc->lPitch = sLockedRect.Pitch;
		pDesc->lpSurface = sLockedRect.pBits;

		m_pD2GI->OnBackBufferLock(!(dwFlags & DDLOCK_WRITEONLY), &sLockedRect);

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GIBackBufferSurface::Unlock(LPRECT)
{
	m_pSurface->UnlockRect();
	return DD_OK;
}


HRESULT D2GIBackBufferSurface::IsLost()
{
	return DD_OK;
}


HRESULT D2GIBackBufferSurface::AddAttachedSurface(D3D7::LPDIRECTDRAWSURFACE7 pSurf)
{
	if (((D2GISurface*)pSurf)->GetType() == ST_ZBUFFER)
		return DD_OK;

	return DDERR_GENERIC;
}


HRESULT D2GIBackBufferSurface::Blt(LPRECT pDestRT, D3D7::LPDIRECTDRAWSURFACE7 pSrc, LPRECT pSrcRT, DWORD dwFlags, D3D7::LPDDBLTFX lpFX)
{
	D2GISurface* pSurf = (D2GISurface*)pSrc;

	if (pSrc == NULL || pSurf->GetType() != ST_SYSMEM)
		return DDERR_GENERIC;

	m_pD2GI->OnSysMemSurfaceBltOnBackBuffer((D2GISystemMemorySurface*)pSurf, pSrcRT, this, pDestRT);

	return DD_OK;
}
