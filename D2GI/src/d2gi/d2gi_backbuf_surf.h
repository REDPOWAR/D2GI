#pragma once

#include "d2gi_surface.h"


class D2GIBackBufferSurface : public D2GISurface
{
	DWORD m_dwWidth, m_dwHeight, m_dwBPP;
	D3D7::DDPIXELFORMAT m_sPixelFormat;

	D3D9::IDirect3DTexture9* m_pTexture;
	D3D9::IDirect3DSurface9* m_pSurface;

public:
	D2GIBackBufferSurface(D2GI*);
	virtual ~D2GIBackBufferSurface();

	virtual SURFACETYPE GetType() { return ST_BACKBUFFER; }
	virtual VOID ReleaseResource();
	virtual VOID LoadResource();

	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(Unlock)(LPRECT);
	STDMETHOD(IsLost)();

	D3D9::IDirect3DSurface9* GetD3D9Surface() { return m_pSurface; }
};
