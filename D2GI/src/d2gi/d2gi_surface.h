#pragma once

#include "../ddraw/dd_surface.h"

#include "d2gi_common.h"
#include "d2gi_resource.h"
#include "d2gi_enums.h"


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


class D2GISurface : public SurfaceProxy, public D2GIResource
{
protected:
	DWORD m_dwWidth, m_dwHeight, m_dwBPP;
	D3D7::DDPIXELFORMAT m_sDD7PixelFormat;
	D2GIPIXELFORMAT m_eD2GIPixelFormat;
public:
	D2GISurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GISurface();

	virtual SURFACETYPE GetType() = 0;

	DWORD GetWidth() { return m_dwWidth; }
	DWORD GetHeight() { return m_dwHeight; }
	DWORD GetBPP() { return m_dwBPP; }
	D3D7::DDPIXELFORMAT* GetDD7PixelFormat() { return &m_sDD7PixelFormat; }
	D2GIPIXELFORMAT GetD2GIPixelFormat() { return m_eD2GIPixelFormat; }
};
