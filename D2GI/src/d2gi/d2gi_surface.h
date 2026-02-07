#pragma once

#include "../ddraw/dd_surface.h"

#include "d2gi_common.h"
#include "d2gi_resource.h"
#include "d2gi_enums.h"

#include "../common/implements_proxy.hpp"


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
	DWORD m_dwWidth, m_dwHeight, m_dwBPP;
	D3D7::DDPIXELFORMAT m_sDD7PixelFormat;
	D2GIPIXELFORMAT m_eD2GIPixelFormat;

	D3D9::D3DLOCKED_RECT m_LastColorKeyLock {}; // Used for color key updates

	D3D7::DDCOLORKEY m_sColorKey {};
	bool m_bColorKeySet = false;

public:
	D2GISurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GISurface() override;

	IFACEMETHOD(IsLost)() override final;
	IFACEMETHOD(SetColorKey)(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK) override;

	virtual SURFACETYPE GetType() const = 0;

	DWORD GetWidth() const { return m_dwWidth; }
	DWORD GetHeight() const { return m_dwHeight; }
	DWORD GetBPP() const { return m_dwBPP; }
	const D3D7::DDPIXELFORMAT* GetDD7PixelFormat() const { return &m_sDD7PixelFormat; }
	D2GIPIXELFORMAT GetD2GIPixelFormat() const { return m_eD2GIPixelFormat; }

	bool HasColorKeyConversion() const { return m_bColorKeySet && m_eD2GIPixelFormat == D2GIPF_16_565; }
	DWORD GetOriginalColorKeyValue() const { return m_sColorKey.dwColorSpaceLowValue; }
	D3D9::D3DFORMAT GetEffectiveD3DFormat() const;
};
