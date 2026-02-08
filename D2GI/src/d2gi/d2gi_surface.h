#pragma once

#include "../ddraw/dd_surface.h"

#include "d2gi_common.h"
#include "d2gi_resource.h"
#include "d2gi_enums.h"

#include "../common/implements_proxy.hpp"

#include <memory>
#include <optional>


enum SURFACETYPE
{
	ST_PRIMARY_FLIPPABLE,
	ST_BACKBUFFER,
	ST_PRIMARY_SINGLE,
	ST_ZBUFFER,
	ST_TEXTURE,
	ST_SYSMEM,
	ST_MIPMAP,
};


class D2GISurface : public ImplementsProxy<SurfaceProxy>, public D2GIResource
{
protected:
	uint32_t m_dwWidth, m_dwHeight, m_dwBPP;
	const D3D7::DDPIXELFORMAT& m_sDD7PixelFormat;
	D2GIPIXELFORMAT m_eD2GIPixelFormat;

	std::unique_ptr<std::byte[]> m_intermediateBuffer; // For 8bpp palettized and color keyed surfaces
	std::optional<uint32_t> m_sColorKey;
	bool m_bSurfaceDirty = false;

public:
	D2GISurface(D2GI*, uint32_t dwW, uint32_t dwH, D2GIPIXELFORMAT);
	virtual ~D2GISurface() override;
	virtual void ReleaseResource(bool bResettingDevice) override;

	IFACEMETHOD(IsLost)() override final;
	IFACEMETHOD(SetColorKey)(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK) override;

	virtual SURFACETYPE GetType() const = 0;

	DWORD GetWidth() const { return m_dwWidth; }
	DWORD GetHeight() const { return m_dwHeight; }
	DWORD GetBPP() const { return m_dwBPP; }
	D2GIPIXELFORMAT GetD2GIPixelFormat() const { return m_eD2GIPixelFormat; }

	bool HasColorKeyConversion() const { return m_sColorKey.has_value() && m_eD2GIPixelFormat == D2GIPF_16_565; }
	uint32_t GetOriginalColorKeyValue() const { return m_sColorKey.value(); }

protected:
	bool NeedsIntermediateBuffer() const { return m_dwBPP == 8 || HasColorKeyConversion(); }
	D3D9::D3DFORMAT GetEffectiveD3DFormat() const;
	void EnsureD3DResourceCreated();
	void ExpandColorKeyToSurface(D3D9::IDirect3DSurface9* pSurface, bool bCanDiscard) const;
};
