#pragma once

#include "d2gi_surface.h"

#include <wrl/client.h>


class D2GIBackBufferSurface : public D2GISurface
{
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> m_pStreamingSurface;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> m_pReadingSurface;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> m_pOffSurface;
	bool m_bLastLockReadOnly = false;

public:
	D2GIBackBufferSurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GIBackBufferSurface() override;

	virtual SURFACETYPE GetType() const override { return ST_BACKBUFFER; }
	virtual void ReleaseResource(bool bResettingDevice) override;
	virtual void LoadResource(bool bResettingDevice) override;

	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE) override;
	STDMETHOD(Unlock)(LPRECT) override;
	STDMETHOD(AddAttachedSurface)(D3D7::LPDIRECTDRAWSURFACE7) override;
	STDMETHOD(Blt)(LPRECT, D3D7::LPDIRECTDRAWSURFACE7, LPRECT, DWORD, D3D7::LPDDBLTFX) override;

	D3D9::IDirect3DSurface9* GetD3D9StreamingSurface() const { return m_pStreamingSurface.Get(); }
	D3D9::IDirect3DSurface9* GetD3D9ReadingSurface() const  { return m_pReadingSurface.Get(); }
};
