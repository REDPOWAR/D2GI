
#include  "../common/logger.h"

#include "d2gi_texture.h"
#include "d2gi_mipmap_surf.h"
#include "d2gi_enums.h"
#include "d2gi_palette.h"


D2GIMipMapSurface::D2GIMipMapSurface(D2GITexture* pParent, UINT uLevelID, D2GIMipMapSurface* pNextSurf,
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pParent->GetD2GI(), dwWidth, dwHeight, eFormat), m_pParent(pParent), m_pNextLevel(pNextSurf)
	, m_uLevelID(uLevelID)
{
}


D2GIMipMapSurface::~D2GIMipMapSurface()
{
}

void D2GIMipMapSurface::ReleaseResource(bool bResettingDevice)
{
	D2GISurface::ReleaseResource(bResettingDevice);

	m_pSurface.Reset();
}

void D2GIMipMapSurface::SetD3D9Surface(Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> pSurf)
{
	m_pSurface = std::move(pSurf);
	m_bSurfaceDirty = true;
}


HRESULT D2GIMipMapSurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE)
{
	if (pRect == NULL)
	{
		ZeroMemory(pDesc, sizeof(*pDesc));
		pDesc->dwSize = sizeof(*pDesc);
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

		const bool bReadOnly = (dwFlags & DDLOCK_READONLY) != 0;
		const bool bWriteOnly = (dwFlags & DDLOCK_WRITEONLY) != 0;
		if (!bReadOnly || bWriteOnly)
		{
			m_bSurfaceDirty = true;
		}

		EnsureD3DResourceCreated();
		if (m_intermediateBuffer)
		{
			pDesc->lpSurface = m_intermediateBuffer.get();
			pDesc->lPitch = m_dwWidth * (m_dwBPP / 8);
			return DD_OK;
		}

		DWORD lockFlags = D3DLOCK_NOSYSLOCK;
		if (bReadOnly)
		{
			lockFlags |= D3DLOCK_READONLY;
		}

		D3D9::D3DLOCKED_RECT sRect;
		if (FAILED(m_pSurface->LockRect(&sRect, nullptr, lockFlags)))
			Logger::Error(TEXT("Failed to lock mip map surface"));

		pDesc->lpSurface = sRect.pBits;
		pDesc->lPitch = sRect.Pitch;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GIMipMapSurface::Unlock(LPRECT)
{
	if (!m_intermediateBuffer)
	{
		m_pSurface->UnlockRect();
	}
	return DD_OK;
}


HRESULT D2GIMipMapSurface::GetAttachedSurface(D3D7::LPDDSCAPS2 pCaps, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	if ((pCaps->dwCaps & DDSCAPS_MIPMAP) && m_pNextLevel != nullptr)
	{
		m_pNextLevel->AddRef();
		*lpSurf = m_pNextLevel;
		return DD_OK;
	}

	Logger::Warning(TEXT("Requested unknown attached surface to mipmap"));
	return DDERR_NOTFOUND;
}

void D2GIMipMapSurface::FlushResourceToGPU()
{
	if (m_bSurfaceDirty)
	{
		if (HasColorKeyConversion())
		{
			ExpandColorKeyToSurface(m_pSurface.Get(), /*bCanDiscard=*/ m_uLevelID == 0);
		}

		m_bSurfaceDirty = false;
	}
}