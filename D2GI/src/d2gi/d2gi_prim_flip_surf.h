#pragma once

#include "d2gi_surface.h"


class D2GIBackBufferSurface;


class D2GIPrimaryFlippableSurface : public D2GISurface
{
	D2GIBackBufferSurface* m_pBackBuffer;
public:
	D2GIPrimaryFlippableSurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GIPrimaryFlippableSurface();

	virtual SURFACETYPE GetType() { return ST_PRIMARY_FLIPPABLE; }
	virtual VOID ReleaseResource();
	virtual VOID LoadResource();

	STDMETHOD(GetAttachedSurface)(D3D7::LPDDSCAPS2, D3D7::LPDIRECTDRAWSURFACE7 FAR*);
	STDMETHOD(GetSurfaceDesc)(D3D7::LPDDSURFACEDESC2);
	STDMETHOD(IsLost)();
	STDMETHOD(Flip)(D3D7::LPDIRECTDRAWSURFACE7, DWORD);
	STDMETHOD(GetFlipStatus)(DWORD);

	D2GIBackBufferSurface* GetBackBufferSurface() { return m_pBackBuffer; }
};
