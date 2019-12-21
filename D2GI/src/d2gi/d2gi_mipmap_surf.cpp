
#include  "../common/logger.h"

#include "d2gi_texture.h"
#include "d2gi_mipmap_surf.h"
#include "d2gi_enums.h"
#include "d2gi_palette.h"


D2GIMipMapSurface::D2GIMipMapSurface(D2GITexture* pParent, UINT uLevelID, D2GIMipMapSurface* pNextSurf,
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pParent->GetD2GI(), dwWidth, dwHeight, eFormat)
{
	m_pSurface = NULL;
	m_pNextLevel = pNextSurf;

	m_pParent = pParent;
	m_pNextLevel = pNextSurf;
	m_uLevelID = uLevelID;

	m_pSurface = NULL;
	m_uDataPitch = (m_dwBPP / 8) * m_dwWidth;
	m_uDataSize = m_uDataPitch * m_dwHeight;
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
	UpdateSurface();
}


HRESULT D2GIMipMapSurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD, HANDLE)
{
	if (pRect == NULL)
	{
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
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = m_sDD7PixelFormat;
		pDesc->lpSurface = m_pData;
		pDesc->lPitch = m_uDataPitch;

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

	if (m_pSurface == NULL)
		return;

	if (m_dwBPP != 16)
		return;

	if (FAILED(m_pSurface->LockRect(&sRect, NULL, 0)))
		Logger::Error(TEXT("Failed to lock mip map surface"));

	if (m_pParent->HasColorKeyConversion())
	{
		INT i, j;

		for (i = 0; i < (INT)m_dwHeight; i++)
		{
			for (j = 0; j < (INT)m_dwWidth; j++)
			{
				UINT16 uSrcColor = *((UINT16*)m_pData + i * m_dwWidth + j);
				UINT32 uDstColor;
				BYTE r, g, b, a;

				r = ((uSrcColor >> 11) & 0x1F) * 255 / 31;
				g = ((uSrcColor >> 5) & 0x3F) * 255 / 63;
				b = (uSrcColor & 0x1F) * 255 / 31;
				a = (uSrcColor == (UINT16)m_pParent->GetOriginalColorKeyValue()) ? 0 : 255;

				uDstColor = (a << 24) | (r << 16) | (g << 8) | b;

				((UINT32*)((BYTE*)sRect.pBits + i * sRect.Pitch))[j] = uDstColor;
			}
		}
	}
	else 
	{
		INT i;

		for (i = 0; i < (INT)m_dwHeight; i++)
			CopyMemory((BYTE*)sRect.pBits + i * sRect.Pitch, (BYTE*)m_pData + i * m_uDataPitch, m_uDataPitch);
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

	Logger::Warning(TEXT("Requested unknown attached surface to mipmap"));
	return DDERR_NOTFOUND;
}


VOID D2GIMipMapSurface::UpdateWithPalette(D2GIPalette* pPal)
{
	D3D9::D3DLOCKED_RECT sLockedRect;
	UINT16* pPalette16 = pPal->GetEntries16();
	INT i, j;

	if (FAILED(m_pSurface->LockRect(&sLockedRect, NULL, D3DLOCK_DISCARD)))
		Logger::Error(TEXT("Failed to lock mipmap surface to update with palette"));

	for (i = 0; i < (INT)m_dwHeight; i++)
		for (j = 0; j < (INT)m_dwWidth; j++)
			((UINT16*)((BYTE*)sLockedRect.pBits + i * sLockedRect.Pitch))[j] = pPalette16[((BYTE*)m_pData)[i * m_dwWidth + j]];

	m_pSurface->UnlockRect();
}
