
#include "d2gi.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_enums.h"
#include "d2gi_palette.h"


D2GISystemMemorySurface::D2GISystemMemorySurface(D2GI* pD2GI, DWORD dwW, DWORD dwH, D3D7::DDPIXELFORMAT* ppf) 
	: D2GISurface(pD2GI), m_dwWidth(dwW), m_dwHeight(dwH), m_dwCKFlags(0),
	m_pTexture(NULL), m_pSurface(NULL), m_pData(NULL)
{
	if (ppf != NULL)
	{
		m_sPixelFormat = *ppf;
		m_dwBPP = ppf->dwRGBBitCount;
	}
	else
	{
		m_dwBPP = m_pD2GI->GetOriginalBPP();

		if (m_dwBPP == 16)
			m_sPixelFormat = g_pf16_1555;
		else
			m_sPixelFormat = g_pf8_Pal;
	}

	LoadResource();
}


D2GISystemMemorySurface::~D2GISystemMemorySurface()
{
	ReleaseResource();
}



HRESULT D2GISystemMemorySurface::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	if (dwFlags & DDCKEY_SRCBLT)
	{
		m_dwCKFlags = dwFlags;
		m_sColorKey = *pCK;

		return DD_OK;
	}

	return DDERR_GENERIC;
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
		if (m_sPixelFormat == g_pf16_565)
			eTextureFormat = D3D9::D3DFMT_R5G6B5;
		else if (m_sPixelFormat == g_pf16_4444)
			eTextureFormat = D3D9::D3DFMT_A4R4G4B4;
		else
			eTextureFormat = D3D9::D3DFMT_A1R5G5B5;
	}
	else if (m_dwBPP == 8)
	{
		m_uDataSize = m_dwWidth * m_dwHeight;
		m_uPitch = m_dwWidth;
		eTextureFormat = D3D9::D3DFMT_A1R5G5B5;
	}

	m_pData = new BYTE[m_uDataSize];
	memset(m_pData, 0xD800, m_uDataSize);

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

		if (m_dwCKFlags & DDCKEY_SRCBLT)
		{
			pDesc->dwFlags |= DDSD_CKSRCBLT;
			pDesc->ddckCKSrcBlt = m_sColorKey;
		}

		pDesc->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = m_sPixelFormat;
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
		INT i;

		m_pSurface->LockRect(&sLockedRect, NULL, 0);

		for (i = 0; i < (INT)m_dwHeight; i++)
			CopyMemory((BYTE*)sLockedRect.pBits + i * sLockedRect.Pitch, 
				(BYTE*)m_pData + i * m_uPitch, sizeof(UINT16) * m_dwWidth);

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


BOOL D2GISystemMemorySurface::HasColorKey()
{
	return !!(m_dwCKFlags & DDCKEY_SRCBLT);
}


DWORD D2GISystemMemorySurface::GetColorKeyValue()
{
	BYTE r, g, b, a;

	r = ((m_sColorKey.dwColorSpaceLowValue >> 10) & 0x1F) * 255 / 32;
	g = ((m_sColorKey.dwColorSpaceLowValue >> 5) & 0x1F) * 255 / 32;
	b = ((m_sColorKey.dwColorSpaceLowValue) & 0x1F) * 255 / 32;
	a = ((m_sColorKey.dwColorSpaceLowValue >> 15) & 0x1) * 255;

	return (r << 24) | (g << 16) | (b << 8) | a;
}
