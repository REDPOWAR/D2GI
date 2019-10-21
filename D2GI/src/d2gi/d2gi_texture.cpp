
#include "d2gi_texture.h"
#include "d2gi_enums.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi.h"


D2GITexture::D2GITexture(D2GI* pD2GI, DWORD dwW, DWORD dwH, DWORD dwMipMapCount) 
	: D2GISurface(pD2GI), m_dwWidth(dwW), m_dwHeight(dwH), 
	m_dwMipMapCount(dwMipMapCount), m_pTexture(NULL), m_pSurface(NULL)
{
	LoadResource();
}


D2GITexture::~D2GITexture()
{
	ReleaseResource();
}


VOID D2GITexture::LoadResource()
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	pDev->CreateTexture(m_dwWidth, m_dwHeight,
		m_dwMipMapCount == 0 ? 1 : m_dwMipMapCount, D3DUSAGE_DYNAMIC, 
		D3D9::D3DFMT_A1R5G5B5, D3D9::D3DPOOL_DEFAULT, &m_pTexture, NULL);

	m_pTexture->GetSurfaceLevel(0, &m_pSurface);
}


VOID D2GITexture::ReleaseResource()
{
	RELEASE(m_pSurface);
	RELEASE(m_pTexture);
}


HRESULT D2GITexture::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	m_dwCKFlags = dwFlags;
	if(pCK != NULL)	
		m_sColorKey = *pCK;
	return DD_OK;
}


HRESULT D2GITexture::IsLost()
{
	return DD_OK;
}


HRESULT D2GITexture::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD, HANDLE)
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

		pDesc->ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = g_pf16_565;
		pDesc->lpSurface = sLockedRect.pBits;
		pDesc->lPitch = sLockedRect.Pitch;
	
		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GITexture::Unlock(LPRECT)
{
	m_pSurface->UnlockRect();
	return DD_OK;
}


HRESULT D2GITexture::Blt(LPRECT pDestRT, D3D7::LPDIRECTDRAWSURFACE7 pSrc, LPRECT pSrcRT, DWORD dwFlags, D3D7::LPDDBLTFX lpFX)
{
	D2GISurface* pSurf = (D2GISurface*)pSrc;

	if (pSrc == NULL || pSurf->GetType() != ST_SYSMEM)
		return DDERR_GENERIC;

	m_pD2GI->OnSysMemSurfaceBltOnTexture((D2GISystemMemorySurface*)pSurf, pSrcRT, this, pDestRT);

	return DD_OK;
}
