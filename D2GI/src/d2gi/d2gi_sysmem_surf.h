#pragma once

#include "d2gi_texture.h"


class D2GISystemMemorySurface : public D2GITexture
{
public:
	D2GISystemMemorySurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT eFormat);
	virtual ~D2GISystemMemorySurface();

	virtual SURFACETYPE GetType() { return ST_SYSMEM; }

	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(GetCaps)(D3D7::LPDDSCAPS2);
};
