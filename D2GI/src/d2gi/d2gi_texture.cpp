
#include "../common/utils.h"
#include "../common/logger.h"

#include "d2gi_texture.h"
#include "d2gi_enums.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi.h"

#include <d3dcommon.h>


D2GITexture::D2GITexture(D2GI* pD2GI, DWORD dwWidth, DWORD dwHeight, 
	D2GIPIXELFORMAT eFormat, DWORD dwMipMapCount) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
	m_dwMipMapCount = (dwMipMapCount == 0) ? 1 : dwMipMapCount;
	m_lpMipMapLevels = NULL;
	m_pTexture = NULL;

	INT i;

	m_lpMipMapLevels = new D2GIMipMapSurface* [m_dwMipMapCount];
	for (i = (INT)m_dwMipMapCount - 1; i >= 0; i--)
	{
		DWORD dwMipMapWidth, dwMipMapHeight;
		D2GIMipMapSurface* pNextMipMap = (i < (INT)m_dwMipMapCount - 1) ? m_lpMipMapLevels[i + 1] : NULL;

		CalcMipMapLevelSize(m_dwWidth, m_dwHeight, i, &dwMipMapWidth, &dwMipMapHeight);
		m_lpMipMapLevels[i] = new D2GIMipMapSurface(this, i, pNextMipMap, 
			dwMipMapWidth, dwMipMapHeight, m_eD2GIPixelFormat);
	}

	LoadResource(/*bResettingDevice=*/ false);
}


D2GITexture::~D2GITexture()
{
	INT i;

	ReleaseResource(/*bResettingDevice=*/ false);

	for (i = 0; i < (INT)m_dwMipMapCount; i++)
		RELEASE(m_lpMipMapLevels[i]);

	DEL(m_lpMipMapLevels);
}


VOID D2GITexture::LoadResource(bool bResettingDevice)
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	D3D9::D3DFORMAT eFormat = GetEffectiveD3DFormat();
	DWORD i;

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


VOID D2GITexture::ReleaseResource(bool bResettingDevice)
{
	INT i;

	for (i = 0; i < (INT)m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->SetD3D9Surface(NULL);

	RELEASE(m_pTexture);
}


HRESULT D2GITexture::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	D2GISurface::SetColorKey(dwFlags, pCK);

	INT i;

	// Keep the texture name
#ifdef _DEBUG
	char textureName[256];
	DWORD len = std::size(textureName);
	const HRESULT getNameHresult = m_pTexture->GetPrivateData(WKPDID_D3DDebugObjectName, textureName, &len);
#endif

	ReleaseResource(/*bResettingDevice=*/ false);
	LoadResource(/*bResettingDevice=*/ false);

#ifdef _DEBUG
	if (SUCCEEDED(getNameHresult))
	{
		m_pTexture->SetPrivateData(WKPDID_D3DDebugObjectName, textureName, len, 0);
	}
#endif

	for (i = 0; i < (INT)m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->UpdateSurface();

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


/*VOID D2GITexture::UpdateWithPalette(D2GIPalette* pPal)
{
	INT i;

	for (i = 0; i < (INT)m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->UpdateWithPalette(pPal);
}*/


IFACEMETHODIMP D2GITexture::SetPrivateData(REFGUID refguid, LPVOID pData, DWORD SizeOfData, DWORD Flags)
{
	return m_pTexture->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

IFACEMETHODIMP D2GITexture::GetPrivateData(REFGUID refguid, LPVOID pData, LPDWORD pSizeOfData)
{
	return m_pTexture->GetPrivateData(refguid, pData, pSizeOfData);
}

IFACEMETHODIMP D2GITexture::FreePrivateData(REFGUID refguid)
{
	return m_pTexture->FreePrivateData(refguid);
}
