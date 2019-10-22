
#include "d2gi_texture.h"
#include "d2gi_mipmap_surf.h"
#include "d2gi_enums.h"


D2GIMipMapSurface::D2GIMipMapSurface(D2GI* pD2GI, D2GITexture* pParent, UINT uID, D2GIMipMapSurface* pNextSurf) 
	: D2GISurface(pD2GI), m_pParent(pParent), m_uLevelID(uID), m_pSurface(NULL), m_pNextLevel(pNextSurf)
{

}


D2GIMipMapSurface::~D2GIMipMapSurface()
{
	RELEASE(m_pSurface);
}


VOID D2GIMipMapSurface::SetD3D9Surface(D3D9::IDirect3DSurface9* pSurf)
{
	if (pSurf == m_pSurface)
		return;

	RELEASE(m_pSurface);
	m_pSurface = pSurf;
}


HRESULT D2GIMipMapSurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD, HANDLE)
{
	if (pRect == NULL)
	{
		D3D9::D3DLOCKED_RECT sLockedRect;
		D3D9::D3DSURFACE_DESC sSurfDesc;

		m_pSurface->LockRect(&sLockedRect, NULL, (m_uLevelID == 0) ? D3DLOCK_DISCARD : 0);
		m_pSurface->GetDesc(&sSurfDesc);

		ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
		pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
		pDesc->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;

		pDesc->dwMipMapCount = m_pParent->GetMipMapCount() - m_uLevelID;

		/*if (m_dwCKFlags & DDCKEY_SRCBLT)
		{
			pDesc->dwFlags |= DDSD_CKSRCBLT;
			pDesc->ddckCKSrcBlt = m_sColorKey;
		}*/

		pDesc->ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
		pDesc->dwWidth = sSurfDesc.Width;
		pDesc->dwHeight = sSurfDesc.Height;
		pDesc->ddpfPixelFormat = g_pf16_565;
		pDesc->lpSurface = sLockedRect.pBits;
		pDesc->lPitch = sLockedRect.Pitch;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GIMipMapSurface::Unlock(LPRECT)
{
	m_pSurface->UnlockRect();
	return DD_OK;
}


HRESULT D2GIMipMapSurface::GetAttachedSurface(D3D7::LPDDSCAPS2 pCaps, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	if ((pCaps->dwCaps & DDSCAPS_MIPMAP) && m_pNextLevel != NULL)
	{
		m_pNextLevel->AddRef();
		*lpSurf = (D3D7::IDirectDrawSurface7*)m_pNextLevel;
		return DD_OK;
	}

	return DDERR_NOTFOUND;
}
