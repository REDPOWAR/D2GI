#pragma once

#include "d2gi_surface.h"
#include "d2gi_palette.h"


class D2GIPrimarySingleSurface : public D2GISurface
{
	D2GIPalette* m_pPalette;
public:
	D2GIPrimarySingleSurface(D2GI*, DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat);
	virtual ~D2GIPrimarySingleSurface();

	virtual SURFACETYPE GetType() const override { return ST_PRIMARY_SINGLE; }
	virtual void LoadResource(bool bResettingDevice) override {};

	STDMETHOD(GetSurfaceDesc)(D3D7::LPDDSURFACEDESC2);
	STDMETHOD(SetPalette)(D3D7::LPDIRECTDRAWPALETTE);
	STDMETHOD(Blt)(LPRECT, D3D7::LPDIRECTDRAWSURFACE7, LPRECT, DWORD, D3D7::LPDDBLTFX);

	D2GIPalette* GetPalette() { return m_pPalette; }
};
