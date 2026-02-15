
#include "../common/utils.h"
#include "../common/logger.h"

#include "d2gi_texture.h"
#include "d2gi_enums.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi.h"

#include <d3dcommon.h>

#include <new>


D2GITexture::D2GITexture(D2GI* pD2GI, DWORD dwWidth, DWORD dwHeight, 
	D2GIPIXELFORMAT eFormat, DWORD dwMipMapCount) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat), m_dwMipMapCount((dwMipMapCount == 0) ? 1 : dwMipMapCount)
{
	m_lpMipMapLevels.reset(new(std::nothrow) Microsoft::WRL::ComPtr<D2GIMipMapSurface>[m_dwMipMapCount]);
	for (INT i = (INT)m_dwMipMapCount - 1; i >= 0; i--)
	{
		DWORD dwMipMapWidth, dwMipMapHeight;
		D2GIMipMapSurface* pNextMipMap = (i < (INT)m_dwMipMapCount - 1) ? m_lpMipMapLevels[i + 1].Get() : nullptr;

		CalcMipMapLevelSize(m_dwWidth, m_dwHeight, i, &dwMipMapWidth, &dwMipMapHeight);
		m_lpMipMapLevels[i].Attach(new D2GIMipMapSurface(this, i, pNextMipMap, 
			dwMipMapWidth, dwMipMapHeight, m_eD2GIPixelFormat));
	}
}


D2GITexture::~D2GITexture()
{
}


void D2GITexture::LoadResource(bool bResettingDevice)
{
	if (bResettingDevice && m_pLastBlitSource != nullptr)
	{
		EnsureD3DResourceCreated();
		GetD3D9Device()->UpdateSurface(m_pLastBlitSource.Get(), nullptr, GetD3D9Surface(), nullptr);
	}
}


void D2GITexture::ReleaseResource(bool bResettingDevice)
{
	// Managed pool textures don't need resetting for the device loss
	if (bResettingDevice && !IsDynamicTexture())
	{
		return;
	}

	for (size_t i = 0; i < m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->ReleaseResource(bResettingDevice);

	m_pTexture.Reset();
	if (!bResettingDevice)
	{
		m_pLastBlitSource.Reset();
	}
}

IFACEMETHODIMP D2GITexture::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	const HRESULT hr = D2GISurface::SetColorKey(dwFlags, pCK);
	if (SUCCEEDED(hr))
	{
		// Propagate a new color key to all mipmap surfaces too, for convenience
		for (size_t i = 0; i < m_dwMipMapCount; i++)
			m_lpMipMapLevels[i]->SetColorKey(dwFlags, pCK);
	}
	return hr;
}


HRESULT D2GITexture::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE h)
{
	EnsureD3DResourceCreated();
	return m_lpMipMapLevels[0]->Lock(pRect, pDesc, dwFlags, h);
}


HRESULT D2GITexture::Unlock(LPRECT pRect)
{
	EnsureD3DResourceCreated();
	return m_lpMipMapLevels[0]->Unlock(pRect);
}


HRESULT D2GITexture::Blt(LPRECT pDestRT, D3D7::LPDIRECTDRAWSURFACE7 pSrc, LPRECT pSrcRT, DWORD dwFlags, D3D7::LPDDBLTFX lpFX)
{
	D2GISurface* pSurf = (D2GISurface*)pSrc;

	if (pSrc == NULL || pSurf->GetType() != ST_SYSMEM)
		return DDERR_GENERIC;

	D2GISystemMemorySurface* pSystemMemSurf = static_cast<D2GISystemMemorySurface*>(pSurf);
	SetBlitSource(pSystemMemSurf);

	FlushResourceToGPU();
	m_pD2GI->OnSysMemSurfaceBltOnTexture(pSystemMemSurf, pSrcRT, this, pDestRT);

	return DD_OK;
}


D3D9::IDirect3DSurface9* D2GITexture::GetD3D9Surface()
{
	FlushResourceToGPU();
	return m_lpMipMapLevels[0]->GetD3D9Surface();
}

D3D9::IDirect3DTexture9* D2GITexture::GetD3D9Texture()
{
	FlushResourceToGPU();
	return m_pTexture.Get();
}

void D2GITexture::FlushResourceToGPU()
{
	EnsureD3DResourceCreated();

	for (size_t i = 0; i < m_dwMipMapCount; i++)
		m_lpMipMapLevels[i]->FlushResourceFromParent();
}

void D2GITexture::EnsureD3DResourceCreated()
{
	if (m_pTexture)
	{
		return;
	}

	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	D3D9::D3DFORMAT eFormat = GetEffectiveD3DFormat();

	DWORD Usage;
	D3D9::D3DPOOL Pool;
	if (IsDynamicTexture())
	{
		Usage = D3DUSAGE_DYNAMIC;
		Pool = D3D9::D3DPOOL_DEFAULT;
	}
	else
	{
		Usage = 0;
		Pool = D3D9::D3DPOOL_MANAGED;
	}

	if (FAILED(pDev->CreateTexture(m_dwWidth, m_dwHeight,
		m_dwMipMapCount, Usage,
		eFormat, Pool, &m_pTexture, nullptr)))
		Logger::Error(TEXT("Failed to create texture"));

	for (size_t i = 0; i < m_dwMipMapCount; i++)
	{
		Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> pSurf;

		if (FAILED(m_pTexture->GetSurfaceLevel(i, pSurf.GetAddressOf())))
			Logger::Error(TEXT("Failed to get surface for texture"));
		m_lpMipMapLevels[i]->SetD3D9Surface(std::move(pSurf));
	}
}

void D2GITexture::SetBlitSource(const D2GISystemMemorySurface* pBlitSource)
{
	// If the texture was already created as non-blittable before,
	// we have no choice but to discard it. This seems to only be the case for the rain texture, though.
	if (!m_pLastBlitSource)
	{
		m_pTexture.Reset();
	}
	m_pLastBlitSource = pBlitSource->GetSystemMemSurface();
}

HRESULT D2GITexture::GetAttachedSurface(D3D7::LPDDSCAPS2 pCaps, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	EnsureD3DResourceCreated();
	return m_lpMipMapLevels[0]->GetAttachedSurface(pCaps, lpSurf);
}


HRESULT D2GITexture::GetSurfaceDesc(D3D7::LPDDSURFACEDESC2 pDesc)
{
	ZeroMemory(pDesc, sizeof(*pDesc));
	pDesc->dwSize = sizeof(*pDesc);
	pDesc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
	pDesc->dwMipMapCount = m_dwMipMapCount;
	pDesc->dwWidth = m_dwWidth;
	pDesc->dwHeight = m_dwHeight;
	pDesc->ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
	pDesc->ddpfPixelFormat = m_sDD7PixelFormat;

	return DD_OK;
}


IFACEMETHODIMP D2GITexture::SetPrivateData(REFGUID refguid, LPVOID pData, DWORD SizeOfData, DWORD Flags)
{
	EnsureD3DResourceCreated();
	return m_pTexture->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

IFACEMETHODIMP D2GITexture::GetPrivateData(REFGUID refguid, LPVOID pData, LPDWORD pSizeOfData)
{
	EnsureD3DResourceCreated();
	return m_pTexture->GetPrivateData(refguid, pData, pSizeOfData);
}

IFACEMETHODIMP D2GITexture::FreePrivateData(REFGUID refguid)
{
	EnsureD3DResourceCreated();
	return m_pTexture->FreePrivateData(refguid);
}
