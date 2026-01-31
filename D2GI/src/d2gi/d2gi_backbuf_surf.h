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
	virtual ~D2GIBackBufferSurface() override;

	virtual SURFACETYPE GetType() override { return ST_BACKBUFFER; }
	virtual VOID ReleaseResource() override;
	virtual VOID LoadResource() override;

	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE) override;
	STDMETHOD(Unlock)(LPRECT) override;
	STDMETHOD(IsLost)() override;
	STDMETHOD(AddAttachedSurface)(D3D7::LPDIRECTDRAWSURFACE7) override;
	STDMETHOD(Blt)(LPRECT, D3D7::LPDIRECTDRAWSURFACE7, LPRECT, DWORD, D3D7::LPDDBLTFX) override;

	D3D9::IDirect3DSurface9* GetD3D9StreamingSurface() const { return m_pStreamingSurface; }
	D3D9::IDirect3DSurface9* GetD3D9ReadingSurface() const  { return m_pReadingSurface; }
};
