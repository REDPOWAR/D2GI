#pragma once

#include "d2gi_surface.h"


class D2GIBackBufferSurface : public D2GISurface
{
	D3D9::IDirect3DTexture9* m_pStreamingTexture;
	D3D9::IDirect3DSurface9* m_pStreamingSurface;
	D3D9::IDirect3DSurface9* m_pReadingSurface;
	D3D9::IDirect3DSurface9* m_pOffSurface;
	BOOL m_bLastLockReadOnly;

public:
	D2GIBackBufferSurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GIBackBufferSurface();

	virtual SURFACETYPE GetType() { return ST_BACKBUFFER; }
	virtual VOID ReleaseResource();
	virtual VOID LoadResource();

	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(Unlock)(LPRECT);
	STDMETHOD(IsLost)();
	STDMETHOD(AddAttachedSurface)(D3D7::LPDIRECTDRAWSURFACE7);
	STDMETHOD(Blt)(LPRECT, D3D7::LPDIRECTDRAWSURFACE7, LPRECT, DWORD, D3D7::LPDDBLTFX);

	D3D9::IDirect3DSurface9* GetD3D9StreamingSurface() { return m_pStreamingSurface; }
	D3D9::IDirect3DSurface9* GetD3D9ReadingSurface() { return m_pReadingSurface; }
};
