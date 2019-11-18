
#include "d2gi_backbuf_surf.h"
#include "d2gi.h"
#include "d2gi_enums.h"


D2GIBackBufferSurface::D2GIBackBufferSurface(D2GI* pD2GI, DWORD dwWidth, 
	DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
	m_pStreamingTexture = NULL;
	m_pStreamingSurface = NULL;
	m_pReadingSurface = NULL;
	m_pOffSurface = NULL;

	LoadResource();
}


D2GIBackBufferSurface::~D2GIBackBufferSurface()
{
	ReleaseResource();
}


VOID D2GIBackBufferSurface::ReleaseResource()
{
	RELEASE(m_pStreamingTexture);
	RELEASE(m_pStreamingSurface);
	RELEASE(m_pReadingSurface);
	RELEASE(m_pOffSurface);
}


VOID D2GIBackBufferSurface::LoadResource()
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	pDev->CreateTexture(m_dwWidth, m_dwHeight, 1, D3DUSAGE_DYNAMIC, g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat], 
		D3D9::D3DPOOL_DEFAULT, &m_pStreamingTexture, NULL);
	m_pStreamingTexture->GetSurfaceLevel(0, &m_pStreamingSurface);

	pDev->CreateRenderTarget(m_dwWidth, m_dwHeight, g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat],
		D3D9::D3DMULTISAMPLE_NONE, 0, FALSE, &m_pReadingSurface, NULL);

	pDev->CreateOffscreenPlainSurface(m_dwWidth, m_dwHeight, 
		g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat], D3D9::D3DPOOL_SYSTEMMEM, &m_pOffSurface, NULL);
}


HRESULT D2GIBackBufferSurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE)
{
	if (pRect == NULL)
	{
		m_bLastLockReadOnly = !(dwFlags & DDLOCK_WRITEONLY);

		m_pD2GI->OnBackBufferLock(m_bLastLockReadOnly);

		D3D9::D3DLOCKED_RECT sLockedRect;

		if (m_bLastLockReadOnly)	
		{
			GetD3D9Device()->GetRenderTargetData(m_pReadingSurface, m_pOffSurface);
			m_pOffSurface->LockRect(&sLockedRect, NULL, D3DLOCK_READONLY);
		}else
			m_pStreamingSurface->LockRect(&sLockedRect, NULL, D3DLOCK_DISCARD);
		
		ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
		pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
		pDesc->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;
		pDesc->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = m_sDD7PixelFormat;
		pDesc->lPitch = sLockedRect.Pitch;
		pDesc->lpSurface = sLockedRect.pBits;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GIBackBufferSurface::Unlock(LPRECT)
{
	if (m_bLastLockReadOnly)
		m_pOffSurface->UnlockRect();
	else
		m_pStreamingSurface->UnlockRect();
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


HRESULT D2GIBackBufferSurface::Blt(LPRECT pDestRT, D3D7::LPDIRECTDRAWSURFACE7 pSrc, LPRECT pSrcRT, DWORD dwFlags, D3D7::LPDDBLTFX pFX)
{
	if (dwFlags & DDBLT_COLORFILL)
	{
		// TODO: this color fill must affect backbuffer locked data
		// while streaming video playback

		m_pD2GI->OnColorFillOnBackBuffer(pFX->dwFillColor, pDestRT);
		return DD_OK;
	}

	D2GISurface* pSurf = (D2GISurface*)pSrc;

	if (pSrc == NULL || pSurf->GetType() != ST_SYSMEM)
		return DDERR_GENERIC;

	m_pD2GI->OnSysMemSurfaceBltOnBackBuffer((D2GISystemMemorySurface*)pSurf, pSrcRT, this, pDestRT);

	return DD_OK;
}
