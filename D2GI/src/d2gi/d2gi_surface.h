#pragma once

#include "../ddraw/dd_surface.h"


class D2GISurface : public SurfaceProxy
{
	D2GISurface* m_pBackBuffer;
	D2GISurface* m_pZBuffer;
	D2GISurface* m_pMipMap;
public:
	D2GISurface(D3D7::IDirectDrawSurface7*);
	virtual ~D2GISurface();

	STDMETHOD(Blt)(LPRECT, D3D7::LPDIRECTDRAWSURFACE7, LPRECT, DWORD, D3D7::LPDDBLTFX);
	STDMETHOD(Flip)(D3D7::LPDIRECTDRAWSURFACE7, DWORD);
	STDMETHOD(GetAttachedSurface)(D3D7::LPDDSCAPS2, D3D7::LPDIRECTDRAWSURFACE7 FAR*);
	STDMETHOD(AddAttachedSurface)(D3D7::LPDIRECTDRAWSURFACE7);
};
