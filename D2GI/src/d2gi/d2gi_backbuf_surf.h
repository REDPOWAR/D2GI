#pragma once

#include "d2gi_surface.h"

#include <wrl/client.h>


class D2GIBackBufferSurface : public D2GISurface
{
	Microsoft::WRL::ComPtr<D3D9::IDirect3DTexture9> writing_texture_;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> writing_surface_;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> reading_surface_;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> intermediate_surface_;
	D3D9::IDirect3DSurface9* locked_surface_ = nullptr;
	bool is_in_multipart_writing_ = false;

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

	D3D9::IDirect3DTexture9* GetD3D9WritingTexture() const { return writing_texture_.Get(); }
	D3D9::IDirect3DSurface9* GetD3D9WritingSurface() const { return writing_surface_.Get(); }
	D3D9::IDirect3DSurface9* GetD3D9ReadingSurface() const { return reading_surface_.Get(); }
	void OnFlip();
};
