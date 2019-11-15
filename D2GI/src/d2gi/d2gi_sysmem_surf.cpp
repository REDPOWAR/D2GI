
#include "d2gi.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_enums.h"
#include "d2gi_palette.h"


D2GISystemMemorySurface::D2GISystemMemorySurface(D2GI* pD2GI, 
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
	m_bColorKeySet = FALSE;
	m_pTexture = NULL;
	m_pSurface = NULL;
	m_pData = NULL;

	LoadResource();
}


D2GISystemMemorySurface::~D2GISystemMemorySurface()
{
	ReleaseResource();
}



HRESULT D2GISystemMemorySurface::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	if (!(dwFlags & DDCKEY_SRCBLT))
		return DDERR_GENERIC;

	if (pCK != NULL)
	{
		m_sColorKey = *pCK;
		m_bColorKeySet = TRUE;
	}
	else
		m_bColorKeySet = FALSE;

	ReleaseResource();
	LoadResource();
	return DD_OK;	
}


VOID D2GISystemMemorySurface::ReleaseResource()
{
	DEL(m_pData);
	RELEASE(m_pSurface);
	RELEASE(m_pTexture);
}


VOID D2GISystemMemorySurface::LoadResource()
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	D3D9::D3DFORMAT eTextureFormat;

	if (m_dwBPP == 16)
	{
		m_uDataSize = sizeof(UINT16) * m_dwWidth * m_dwHeight;
		m_uPitch = sizeof(UINT16) * m_dwWidth;
		eTextureFormat = g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat];
		if (HasColorKeyConversion())
			eTextureFormat = D3D9::D3DFMT_A8R8G8B8;
	}
	else if (m_dwBPP == 8)
	{
		m_uDataSize = m_dwWidth * m_dwHeight;
		m_uPitch = m_dwWidth;
		eTextureFormat = D3D9::D3DFMT_A1R5G5B5;
	}

	m_pData = new BYTE[m_uDataSize];

	pDev->CreateTexture(m_dwWidth, m_dwHeight, 1, D3DUSAGE_DYNAMIC, 
		eTextureFormat, D3D9::D3DPOOL_DEFAULT, &m_pTexture, NULL);

	m_pTexture->GetSurfaceLevel(0, &m_pSurface);
}


HRESULT D2GISystemMemorySurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE)
{
	if (pRect == NULL)
	{
		ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
		pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
		pDesc->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;

		if (m_bColorKeySet)
		{
			pDesc->dwFlags |= DDSD_CKSRCBLT;
			pDesc->ddckCKSrcBlt = m_sColorKey;
		}

		pDesc->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = m_sDD7PixelFormat;
		pDesc->lpSurface = m_pData;
		pDesc->lPitch = m_uPitch;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GISystemMemorySurface::Unlock(LPRECT)
{
	if (m_dwBPP == 16)
	{
		D3D9::D3DLOCKED_RECT sLockedRect;
		INT i, j;

		m_pSurface->LockRect(&sLockedRect, NULL, 0);

		if (HasColorKeyConversion())
		{

			for (i = 0; i < m_dwHeight; i++)
			{
				for (j = 0; j < m_dwWidth; j++)
				{
					UINT16 uSrcColor = *((UINT16*)m_pData + i * m_dwWidth + j);
					UINT32 uDstColor;
					BYTE r, g, b, a;

					r = ((uSrcColor >> 11) & 0x1F) * 255 / 31;
					g = ((uSrcColor >> 5) & 0x3F) * 255 / 63;
					b = (uSrcColor & 0x1F) * 255 / 31;
					a = (uSrcColor == (UINT16)m_sColorKey.dwColorSpaceLowValue) ? 0 : 255;

					uDstColor = (a << 24) | (r << 16) | (g << 8) | b;

					((UINT32*)((BYTE*)sLockedRect.pBits + i * sLockedRect.Pitch))[j] = uDstColor;
				}
			}
		}
		else
		{
			for (i = 0; i < (INT)m_dwHeight; i++)
				CopyMemory((BYTE*)sLockedRect.pBits + i * sLockedRect.Pitch,
				(BYTE*)m_pData + i * m_uPitch, sizeof(UINT16) * m_dwWidth);
		}

		m_pSurface->UnlockRect();
	}
	return DD_OK;
}



HRESULT D2GISystemMemorySurface::IsLost()
{
	return DD_OK;
}


VOID D2GISystemMemorySurface::UpdateWithPalette(D2GIPalette* pPal)
{
	D3D9::D3DLOCKED_RECT sLockedRect;
	UINT16* pPalette16 = pPal->GetEntries16();
	INT i, j;

	m_pSurface->LockRect(&sLockedRect, NULL, D3DLOCK_DISCARD);
	for (i = 0; i < m_dwHeight; i++)
		for (j = 0; j < m_dwWidth; j++)
			((UINT16*)((BYTE*)sLockedRect.pBits + i * sLockedRect.Pitch))[j] = pPalette16[((BYTE*)m_pData)[i * m_dwWidth + j]];
	m_pSurface->UnlockRect();
}


HRESULT D2GISystemMemorySurface::GetCaps(D3D7::LPDDSCAPS2 pCaps)
{
	ZeroMemory(pCaps, sizeof(D3D7::DDSCAPS2));
	pCaps->dwCaps = DDSCAPS_SYSTEMMEMORY;

	return DD_OK;
}


BOOL D2GISystemMemorySurface::HasColorKeyConversion()
{
	return m_bColorKeySet && m_eD2GIPixelFormat == D2GIPF_16_565;
}


DWORD D2GISystemMemorySurface::GetColorKeyValue()
{
	BYTE r, g, b;

	r = ((m_sColorKey.dwColorSpaceLowValue >> 11) & 0x1F) * 255 / 31;
	g = ((m_sColorKey.dwColorSpaceLowValue >> 5) & 0x3F) * 255 / 63;
	b = ((m_sColorKey.dwColorSpaceLowValue) & 0x1F) * 255 / 31;

	return (r << 24) | (g << 16) | (b << 8) | 255;
}
