
#include "d2gi_texture.h"
#include "d2gi_mipmap_surf.h"
#include "d2gi_enums.h"


D2GIMipMapSurface::D2GIMipMapSurface(D2GI* pD2GI, D2GITexture* pParent, UINT uID, D2GIMipMapSurface* pNextSurf) 
	: D2GISurface(pD2GI), m_pParent(pParent), m_uLevelID(uID), m_pSurface(NULL), m_pNextLevel(pNextSurf)
{
	m_uDataSize = sizeof(UINT16) * pParent->GetWidth() * pParent->GetHeight();
	m_pData = new BYTE[m_uDataSize];
}


D2GIMipMapSurface::~D2GIMipMapSurface()
{
	DEL(m_pData);
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
		D3D9::D3DSURFACE_DESC sSurfDesc;

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
		pDesc->ddpfPixelFormat = *m_pParent->GetPixelFormat();
		pDesc->lpSurface = m_pData;
		pDesc->lPitch = sizeof(UINT16) * sSurfDesc.Width;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GIMipMapSurface::Unlock(LPRECT)
{
	UpdateSurface();
	return DD_OK;
}


VOID D2GIMipMapSurface::UpdateSurface()
{
	D3D9::D3DLOCKED_RECT sRect;
	D3D9::D3DSURFACE_DESC sSurfDesc;
	UINT uPitch;

	m_pSurface->GetDesc(&sSurfDesc);
	m_pSurface->LockRect(&sRect, NULL, 0);
	uPitch = sizeof(UINT16) * sSurfDesc.Width;

	if (m_pParent->HasColorKey() && *m_pParent->GetPixelFormat() == g_pf16_565)
	{
		INT i, j;

		for (i = 0; i < sSurfDesc.Height; i++)
		{
			for (j = 0; j < sSurfDesc.Width; j++)
			{
				UINT16 uSrcColor = *((UINT16*)m_pData + i * sSurfDesc.Width + j);
				UINT32 uDstColor;
				BYTE r, g, b, a;

				r = ((uSrcColor >> 11) & 0x1F) * 255 / 32;
				g = ((uSrcColor >> 5) & 0x3F) * 255 / 64;
				b = (uSrcColor & 0x1F) * 255 / 32;
				a = (uSrcColor == (UINT16)m_pParent->GetOriginalColorKeyValue()) ? 0 : 255;

				uDstColor = (a << 24) | (r << 16) | (g << 8) | b;

				((UINT32*)((BYTE*)sRect.pBits + i * sRect.Pitch))[j] = uDstColor;
			}
		}
	}
	else 
	{
		INT i;

		for (i = 0; i < sSurfDesc.Height; i++)
			CopyMemory((BYTE*)sRect.pBits + i * sRect.Pitch, (BYTE*)m_pData + i * uPitch, uPitch);
	}

	m_pSurface->UnlockRect();
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
