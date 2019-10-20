#pragma once

#include "d2gi_surface.h"



class D2GISystemMemorySurface : public D2GISurface
{
	DWORD m_dwWidth, m_dwHeight;
	D3D7::DDCOLORKEY m_sColorKey;
	DWORD m_dwCKFlags;
public:
	D2GISystemMemorySurface(D2GI*, DWORD dwW, DWORD dwH);
	virtual ~D2GISystemMemorySurface();

	virtual SURFACETYPE GetType() { return ST_SYSMEM; }
	virtual VOID ReleaseResource() {};
	virtual VOID LoadResource() {};

	STDMETHOD(SetColorKey)(DWORD, D3D7::LPDDCOLORKEY);
};
