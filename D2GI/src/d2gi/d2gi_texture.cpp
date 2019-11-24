
#include "../utils.h"
#include "../logger.h"

#include "d2gi_texture.h"
#include "d2gi_enums.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi.h"


D2GITexture::D2GITexture(D2GI* pD2GI, DWORD dwWidth, DWORD dwHeight, 
	D2GIPIXELFORMAT eFormat, DWORD dwMipMapCount) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
	m_dwMipMapCount = (dwMipMapCount == 0) ? 1 : dwMipMapCount;
	m_lpMipMapLevels = NULL;
	m_pTexture = NULL;
	m_bColorKeySet = FALSE;

	INT i;

	m_lpMipMapLevels = new D2GIMipMapSurface* [m_dwMipMapCount];
	for (i = (INT)m_dwMipMapCount - 1; i >= 0; i--)
	{
		DWORD dwMipMapWidth, dwMipMapHeight;
		D2GIMipMapSurface* pNextMipMap = (i < m_dwMipMapCount - 1) ? m_lpMipMapLevels[i + 1] : NULL;

		CalcMipMapLevelSize(m_dwWidth, m_dwHeight, i, &dwMipMapWidth, &dwMipMapHeight);
		m_lpMipMapLevels[i] = new D2GIMipMapSurface(this, i, pNextMipMap, 
			dwMipMapWidth, dwMipMapHeight, m_eD2GIPixelFormat);
	}

	LoadResource();
}


D2GITexture::~D2GITexture()
{
	INT i;

	ReleaseResource();

	for (i = 0; i < (INT)m_dwMipMapCount; i++)
		RELEASE(m_lpMipMapLevels[i]);

	DEL(m_lpMipMapLevels);
}


VOID D2GITexture::LoadResource()
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	D3D9::D3DFORMAT eFormat;
	DWORD dwUsage;
	DWORD i;

	eFormat = g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat];

	if (HasColorKeyConversion())
		eFormat = D3D9::D3DFMT_A8R8G8B8;
	if (m_dwBPP == 8)
		eFormat = D3D9::D3DFMT_R5G6B5;

	if (FAILED(pDev->CreateTexture(m_dwWidth, m_dwHeight,
		m_dwMipMapCount, D3DUSAGE_DYNAMIC,
		eFormat, D3D9::D3DPOOL_DEFAULT, &m_pTexture, NULL)))
		Logger::Error(TEXT("Failed to create texture"));

	for (i = 0; i < m_dwMipMapCount; i++)
	{
		D3D9::IDirect3DSurface9* pSurf;

		if (FAILED(m_pTexture->GetSurfaceLevel(i, &pSurf)))
			Logger::Error(TEXT("Failed to get surface for texture"));
		m_lpMipMapLevels[i]->SetD3D9Surface(pSurf);
	}
}


VOID D2GITexture::ReleaseResource()
{
	INT i;

	for (i = 0; i < m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->SetD3D9Surface(NULL);

	RELEASE(m_pTexture);
}


HRESULT D2GITexture::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	INT i;

	if (!(dwFlags & DDCKEY_SRCBLT))
	{
		Logger::Warning(TEXT("Setting unknown color key for texture"));
		return DDERR_GENERIC;
	}

	if (pCK != NULL)
	{
		m_sColorKey = *pCK;
		m_bColorKeySet = TRUE;
	}
	else
		m_bColorKeySet = FALSE;

	ReleaseResource();
	LoadResource();
	for (i = 0; i < m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->UpdateSurface();

	return DD_OK;
}


HRESULT D2GITexture::IsLost()
{
	return DD_OK;
}


HRESULT D2GITexture::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE h)
{
	return m_lpMipMapLevels[0]->Lock(pRect, pDesc, dwFlags, h);
}


HRESULT D2GITexture::Unlock(LPRECT pRect)
{
	return m_lpMipMapLevels[0]->Unlock(pRect);
}


HRESULT D2GITexture::Blt(LPRECT pDestRT, D3D7::LPDIRECTDRAWSURFACE7 pSrc, LPRECT pSrcRT, DWORD dwFlags, D3D7::LPDDBLTFX lpFX)
{
	D2GISurface* pSurf = (D2GISurface*)pSrc;

	if (pSrc == NULL || pSurf->GetType() != ST_SYSMEM)
		return DDERR_GENERIC;

	m_pD2GI->OnSysMemSurfaceBltOnTexture((D2GISystemMemorySurface*)pSurf, pSrcRT, this, pDestRT);

	return DD_OK;
}


D3D9::IDirect3DSurface9* D2GITexture::GetD3D9Surface()
{
	return m_lpMipMapLevels[0]->GetD3D9Surface();
}


HRESULT D2GITexture::GetAttachedSurface(D3D7::LPDDSCAPS2 pCaps, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return m_lpMipMapLevels[0]->GetAttachedSurface(pCaps, lpSurf);
}


HRESULT D2GITexture::GetSurfaceDesc(D3D7::LPDDSURFACEDESC2 pDesc)
{
	ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
	pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
	pDesc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
	pDesc->dwMipMapCount = m_dwMipMapCount;
	pDesc->dwWidth = m_dwWidth;
	pDesc->dwHeight = m_dwHeight;
	pDesc->ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
	pDesc->ddpfPixelFormat = m_sDD7PixelFormat;

	return DD_OK;
}


BOOL D2GITexture::HasColorKeyConversion()
{
	return m_bColorKeySet && m_eD2GIPixelFormat == D2GIPF_16_565;
}


DWORD D2GITexture::GetOriginalColorKeyValue()
{
	return m_sColorKey.dwColorSpaceLowValue;
}


VOID D2GITexture::UpdateWithPalette(D2GIPalette* pPal)
{
	INT i;

	for (i = 0; i < m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->UpdateWithPalette(pPal);
}


BOOL D2GITexture::CopyFrom(D2GITexture* pSrc)
{
	if (m_dwWidth != pSrc->GetWidth() || m_dwHeight != pSrc->GetHeight())
		return FALSE;

	VOID*                pData;
	D3D9::D3DLOCKED_RECT rt;
	INT                  i, j;

	m_lpMipMapLevels[0]->GetD3D9Surface()->LockRect(&rt, NULL, D3DLOCK_DISCARD);

	for (i = 0; i < m_dwHeight; i++)
		CopyMemory(
			(BYTE*)rt.pBits + i * rt.Pitch,
			(BYTE*)pSrc->m_lpMipMapLevels[0]->GetData() + i * pSrc->m_lpMipMapLevels[0]->GetDataPitch(),
			pSrc->m_lpMipMapLevels[0]->GetDataPitch());

	m_lpMipMapLevels[0]->GetD3D9Surface()->UnlockRect();
}
