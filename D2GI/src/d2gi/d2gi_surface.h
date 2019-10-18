#pragma once

#include "../ddraw/dd_surface.h"


class D2GISurface : public SurfaceProxy
{
	D2GISurface* m_pBackBuffer;
	D2GISurface* m_pZBuffer;
	D2GISurface* m_pMipMap;
public:
	D2GISurface(IDirectDrawSurface7*);
	virtual ~D2GISurface();

	STDMETHOD(Blt)(LPRECT, LPDIRECTDRAWSURFACE7, LPRECT, DWORD, LPDDBLTFX);
	STDMETHOD(Flip)(LPDIRECTDRAWSURFACE7, DWORD);
	STDMETHOD(GetAttachedSurface)(LPDDSCAPS2, LPDIRECTDRAWSURFACE7 FAR*);
	STDMETHOD(AddAttachedSurface)(LPDIRECTDRAWSURFACE7);
};
