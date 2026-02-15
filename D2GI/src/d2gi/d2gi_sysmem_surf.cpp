
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
	D2GISurface::ReleaseResource(bResettingDevice);
	if (!bResettingDevice)
	{
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

		// Non-palettized texture
		D3D9::D3DLOCKED_RECT sRect;
		if (FAILED(m_systemMemSurface->LockRect(&sRect, NULL, lockFlags)))
			Logger::Error(TEXT("Failed to lock the system memory surface"));

		pDesc->lpSurface = sRect.pBits;
		pDesc->lPitch = sRect.Pitch;
		return DD_OK;
	}

	return DDERR_GENERIC;
}

IFACEMETHODIMP D2GISystemMemorySurface::Unlock(LPRECT)
{
	if (!m_intermediateBuffer)
	{
		m_systemMemSurface->UnlockRect();
	}
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
	EnsureD3DResourceCreated();
	if (!m_intermediateBuffer)
	{
		// Not a palettized texture
		return;
	}

	const UINT16* pPalette16 = pPalette->GetEntries16();

	D3D9::D3DLOCKED_RECT sLockedRect;
	if (FAILED(m_systemMemSurface->LockRect(&sLockedRect, NULL, D3DLOCK_NOSYSLOCK)))
		Logger::Error(TEXT("Failed to lock surface to update with palette"));

	const uint8_t* src = reinterpret_cast<const uint8_t*>(m_intermediateBuffer.get());
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
	m_bSurfaceDirty = true;
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

void D2GISystemMemorySurface::EnsureD3DResourceCreated()
{
	if (m_systemMemSurface)
	{
		return;
	}

	D2GISurface::EnsureD3DResourceCreated();

	if (FAILED(GetD3D9Device()->CreateOffscreenPlainSurface(m_dwWidth, m_dwHeight, GetEffectiveD3DFormat(), D3D9::D3DPOOL_SYSTEMMEM, m_systemMemSurface.GetAddressOf(), nullptr)))
		Logger::Error(TEXT("Failed to create surface"));
}

void D2GISystemMemorySurface::FlushResourceToGPU()
{
	EnsureD3DResourceCreated();

	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	if (!m_backingGPUTexture)
	{
		if (FAILED(pDev->CreateTexture(m_dwWidth, m_dwHeight,
			1, D3DUSAGE_DYNAMIC,
			GetEffectiveD3DFormat(), D3D9::D3DPOOL_DEFAULT, m_backingGPUTexture.ReleaseAndGetAddressOf(), nullptr)))
			Logger::Error(TEXT("Failed to create texture"));

		if (FAILED(m_backingGPUTexture->GetSurfaceLevel(0, m_backingGPUSurface.ReleaseAndGetAddressOf())))
			Logger::Error(TEXT("Failed to get surface for texture"));

		m_bSurfaceDirty = true;
	}

	if (m_bSurfaceDirty)
	{
		if (HasColorKeyConversion())
		{
			ExpandColorKeyToSurface(m_systemMemSurface.Get(), /*bCanDiscard=*/ false);
		}

		if (FAILED(pDev->UpdateSurface(m_systemMemSurface.Get(), nullptr, m_backingGPUSurface.Get(), nullptr)))
			Logger::Error(TEXT("Failed to update surface from the system memory"));

		m_bSurfaceDirty = false;
	}
}


IFACEMETHODIMP D2GISystemMemorySurface::SetPrivateData(REFGUID refguid, LPVOID pData, DWORD SizeOfData, DWORD Flags)
{
	EnsureD3DResourceCreated();
	return m_systemMemSurface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

IFACEMETHODIMP D2GISystemMemorySurface::GetPrivateData(REFGUID refguid, LPVOID pData, LPDWORD pSizeOfData)
{
	EnsureD3DResourceCreated();
	return m_systemMemSurface->GetPrivateData(refguid, pData, pSizeOfData);
}

IFACEMETHODIMP D2GISystemMemorySurface::FreePrivateData(REFGUID refguid)
{
	EnsureD3DResourceCreated();
	return m_systemMemSurface->FreePrivateData(refguid);
}
