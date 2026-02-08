

#include "../common/common.h"
#include "../common/logger.h"

#include "d2gi_surface.h"


using namespace D3D7;


D2GISurface::D2GISurface(D2GI* pD2GI, uint32_t dwWidth, uint32_t dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GIResource(pD2GI), m_dwWidth(dwWidth), m_dwHeight(dwHeight), m_eD2GIPixelFormat(eFormat)
	, m_sDD7PixelFormat(g_asD2GIPF_To_DD7PF[eFormat]), m_dwBPP(g_asD2GIPF_To_DD7PF[eFormat].dwRGBBitCount)
{
}


D2GISurface::~D2GISurface()
{
}


void D2GISurface::ReleaseResource(bool bResettingDevice)
{
	if (!bResettingDevice)
	{
		m_intermediateBuffer.reset();
	}
}


IFACEMETHODIMP D2GISurface::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	if (!(dwFlags & DDCKEY_SRCBLT))
	{
		Logger::Warning(TEXT("Setting unknown color key for texture"));
		return DDERR_GENERIC;
	}

	if (pCK != NULL)
	{
		m_sColorKey.emplace(pCK->dwColorSpaceLowValue);
	}
	else
		m_sColorKey.reset();

	return DD_OK;
}


IFACEMETHODIMP D2GISurface::IsLost()
{
	return DD_OK;
}


D3D9::D3DFORMAT D2GISurface::GetEffectiveD3DFormat() const
{
	if (HasColorKeyConversion())
		return D3D9::D3DFMT_A8R8G8B8;
	if (m_dwBPP == 8)
		return D3D9::D3DFMT_R5G6B5;
	return g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat];
}

void D2GISurface::EnsureD3DResourceCreated()
{
	if (NeedsIntermediateBuffer())
	{
		m_intermediateBuffer.reset(new(std::nothrow) std::byte[m_dwWidth * m_dwHeight * (m_dwBPP / 8)]);
	}
}

void D2GISurface::ExpandColorKeyToSurface(D3D9::IDirect3DSurface9* pSurface, bool bCanDiscard) const
{
	DWORD lockFlags = D3DLOCK_NOSYSLOCK;
	if (bCanDiscard)
	{
		lockFlags |= D3DLOCK_DISCARD;
	}

	D3D9::D3DLOCKED_RECT sRect;
	if (FAILED(pSurface->LockRect(&sRect, nullptr, lockFlags)))
		Logger::Error(TEXT("Failed to lock the surface"));

	const uint32_t colorKey = GetOriginalColorKeyValue();

	// src is fully linear, dst is strided by Pitch
	const uint16_t* src = reinterpret_cast<const uint16_t*>(m_intermediateBuffer.get());
	uint32_t* dest = static_cast<uint32_t*>(sRect.pBits);
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

		dest = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(dest) + sRect.Pitch);
	}

	pSurface->UnlockRect();
}
