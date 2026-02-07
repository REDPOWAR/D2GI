
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_enums.h"

#include <new>


D2GISystemMemorySurface::D2GISystemMemorySurface(D2GI* pD2GI, 
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
}


D2GISystemMemorySurface::~D2GISystemMemorySurface()
{
}


void D2GISystemMemorySurface::ReleaseResource(bool bResettingDevice)
{
	if (!bResettingDevice)
	{
		m_paletteSurface.reset();
		m_systemMemSurface.Reset();
	}
	m_backingGPUTexture.Reset();
	m_backingGPUSurface.Reset();
}

IFACEMETHODIMP D2GISystemMemorySurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD dwFlags, HANDLE h)
{
	if (pRect == nullptr)
	{
		ZeroMemory(pDesc, sizeof(*pDesc));
		pDesc->dwSize = sizeof(*pDesc);
		pDesc->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT;

		pDesc->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = m_sDD7PixelFormat;

		EnsureSystemMemResourceCreated();
		if (m_paletteSurface)
		{
			pDesc->lpSurface = m_paletteSurface.get();
			pDesc->lPitch = m_dwWidth;

			m_LastColorKeyLock = {};
			return DD_OK;
		}

		const bool bReadOnly = (dwFlags & DDLOCK_READONLY) != 0;
		DWORD lockFlags = D3DLOCK_NOSYSLOCK;
		if (bReadOnly)
		{
			lockFlags |= D3DLOCK_READONLY;
		}

		// Non-palettized texture
		D3D9::D3DLOCKED_RECT sRect;
		if (FAILED(m_systemMemSurface->LockRect(&sRect, NULL, lockFlags)))
			Logger::Error(TEXT("Failed to lock the system memory surface"));

		if (!bReadOnly && m_bColorKeySet)
		{
			m_LastColorKeyLock = sRect;

			const LONG srcPitch = m_dwWidth * 2;
			const size_t srcSizeInBytes = srcPitch * m_dwHeight;
			const size_t dstSizeInBytes = m_LastColorKeyLock.Pitch * m_dwHeight;

			pDesc->lpSurface = static_cast<char*>(sRect.pBits) + (dstSizeInBytes - srcSizeInBytes);
			pDesc->lPitch = srcPitch;
			return DD_OK;
		}

		m_LastColorKeyLock = {};
		pDesc->lpSurface = sRect.pBits;
		pDesc->lPitch = sRect.Pitch;
		return DD_OK;
	}

	return DDERR_GENERIC;
}

IFACEMETHODIMP D2GISystemMemorySurface::Unlock(LPRECT)
{
	if (m_LastColorKeyLock.pBits != nullptr)
	{
		UpdateColorKey();
	}

	if (!m_paletteSurface)
	{
		m_systemMemSurface->UnlockRect();
	}
	// Palettized textures will be updated later
	m_bTextureDirty = true;
	return DD_OK;
}


IFACEMETHODIMP D2GISystemMemorySurface::GetCaps(D3D7::LPDDSCAPS2 pCaps)
{
	ZeroMemory(pCaps, sizeof(*pCaps));
	pCaps->dwCaps = DDSCAPS_SYSTEMMEMORY;

	return DD_OK;
}

void D2GISystemMemorySurface::UpdateWithPalette(const D2GIPalette* pPalette)
{
	if (!m_paletteSurface)
	{
		// Not a palettized texture
		return;
	}

	EnsureSystemMemResourceCreated();
	const UINT16* pPalette16 = pPalette->GetEntries16();

	D3D9::D3DLOCKED_RECT sLockedRect;
	if (FAILED(m_systemMemSurface->LockRect(&sLockedRect, NULL, D3DLOCK_NOSYSLOCK)))
		Logger::Error(TEXT("Failed to lock surface to update with palette"));

	const uint8_t* src = m_paletteSurface.get();
	uint16_t* dest = static_cast<uint16_t*>(sLockedRect.pBits);
	for (DWORD line = 0; line < m_dwHeight; line++)
	{
		for (DWORD i = 0; i < m_dwWidth; i++)
		{
			dest[i] = pPalette16[src[i]];
		}
		src += m_dwWidth;
		dest = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(dest) + sLockedRect.Pitch);
	}

	m_systemMemSurface->UnlockRect();
	m_bTextureDirty = true;
}

D3D9::IDirect3DSurface9* D2GISystemMemorySurface::RequestGPUSurface()
{
	FlushResourceToGPU();
	return m_backingGPUSurface.Get();
}

D3D9::IDirect3DTexture9* D2GISystemMemorySurface::RequestGPUTexture()
{
	FlushResourceToGPU();
	return m_backingGPUTexture.Get();
}

void D2GISystemMemorySurface::EnsureSystemMemResourceCreated()
{
	if (m_systemMemSurface)
	{
		return;
	}

	if (m_dwBPP == 8)
	{
		m_paletteSurface.reset(new(std::nothrow) uint8_t[m_dwWidth * m_dwHeight]);
	}

	if (FAILED(GetD3D9Device()->CreateOffscreenPlainSurface(m_dwWidth, m_dwHeight, GetEffectiveD3DFormat(), D3D9::D3DPOOL_SYSTEMMEM, m_systemMemSurface.GetAddressOf(), nullptr)))
		Logger::Error(TEXT("Failed to create surface"));
}

void D2GISystemMemorySurface::FlushResourceToGPU()
{
	EnsureSystemMemResourceCreated();

	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	if (!m_backingGPUTexture)
	{
		if (FAILED(pDev->CreateTexture(m_dwWidth, m_dwHeight,
			1, D3DUSAGE_DYNAMIC,
			GetEffectiveD3DFormat(), D3D9::D3DPOOL_DEFAULT, m_backingGPUTexture.ReleaseAndGetAddressOf(), nullptr)))
			Logger::Error(TEXT("Failed to create texture"));

		if (FAILED(m_backingGPUTexture->GetSurfaceLevel(0, m_backingGPUSurface.ReleaseAndGetAddressOf())))
			Logger::Error(TEXT("Failed to get surface for texture"));
	}

	if (m_bTextureDirty)
	{
		if (FAILED(pDev->UpdateSurface(m_systemMemSurface.Get(), nullptr, m_backingGPUSurface.Get(), nullptr)))
			Logger::Error(TEXT("Failed to update surface from the system memory"));

		m_bTextureDirty = false;
	}
}

void D2GISystemMemorySurface::UpdateColorKey()
{
	if (!m_bColorKeySet)
	{
		return;
	}

	const uint32_t colorKey = GetOriginalColorKeyValue();

	const LONG srcPitch = m_dwWidth * 2;
	const size_t srcSizeInBytes = srcPitch * m_dwHeight;
	const size_t dstSizeInBytes = m_LastColorKeyLock.Pitch * m_dwHeight;

	// src is fully linear, dst is strided by Pitch
	const uint16_t* src = reinterpret_cast<const uint16_t*>(static_cast<char*>(m_LastColorKeyLock.pBits) + (dstSizeInBytes - srcSizeInBytes));
	uint32_t* dest = static_cast<uint32_t*>(m_LastColorKeyLock.pBits);
	for (size_t line = 0; line < m_dwHeight; line++)
	{
		for (size_t i = 0; i < m_dwWidth; i++)
		{
			const uint16_t uSrcColor = *src++;

			const uint8_t r = ((uSrcColor >> 11) & 0x1F) * 255 / 31;
			const uint8_t g = ((uSrcColor >> 5) & 0x3F) * 255 / 63;
			const uint8_t b = (uSrcColor & 0x1F) * 255 / 31;
			const uint8_t a = uSrcColor == colorKey ? 0 : 255;

			const uint32_t uDstColor = (a << 24) | (r << 16) | (g << 8) | b;
			dest[i] = uDstColor;
		}

		dest = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(dest) + m_LastColorKeyLock.Pitch);
	}
}

IFACEMETHODIMP D2GISystemMemorySurface::SetPrivateData(REFGUID refguid, LPVOID pData, DWORD SizeOfData, DWORD Flags)
{
	EnsureSystemMemResourceCreated();
	return m_systemMemSurface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

IFACEMETHODIMP D2GISystemMemorySurface::GetPrivateData(REFGUID refguid, LPVOID pData, LPDWORD pSizeOfData)
{
	EnsureSystemMemResourceCreated();
	return m_systemMemSurface->GetPrivateData(refguid, pData, pSizeOfData);
}

IFACEMETHODIMP D2GISystemMemorySurface::FreePrivateData(REFGUID refguid)
{
	EnsureSystemMemResourceCreated();
	return m_systemMemSurface->FreePrivateData(refguid);
}
