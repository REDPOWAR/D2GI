#pragma once

#include "d2gi_surface.h"

#include <wrl/client.h>

class D2GISystemMemorySurface : public D2GISurface
{
private:
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> m_systemMemSurface;

	// Instantiated on demand if the system memory surface is used for blitting.
	Microsoft::WRL::ComPtr<D3D9::IDirect3DTexture9> m_backingGPUTexture;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> m_backingGPUSurface;

public:
	D2GISystemMemorySurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT eFormat);
	virtual ~D2GISystemMemorySurface() override;

	virtual SURFACETYPE GetType() const override { return ST_SYSMEM; }
	virtual void LoadResource(bool bResettingDevice) override { }
	virtual void ReleaseResource(bool bResettingDevice) override;

	IFACEMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE) override;
	IFACEMETHOD(Unlock)(LPRECT) override;
	IFACEMETHOD(GetCaps)(D3D7::LPDDSCAPS2) override;
	IFACEMETHOD(SetPrivateData)(REFGUID, LPVOID, DWORD, DWORD) override;
	IFACEMETHOD(GetPrivateData)(REFGUID, LPVOID, LPDWORD) override;
	IFACEMETHOD(FreePrivateData)(REFGUID) override;

	void UpdateWithPalette(const D2GIPalette* pPalette);

	D3D9::IDirect3DSurface9* GetSystemMemSurface() const { return m_systemMemSurface.Get(); }
	D3D9::IDirect3DSurface9* RequestGPUSurface();
	D3D9::IDirect3DTexture9* RequestGPUTexture();

private:
	void EnsureD3DResourceCreated();
	void FlushResourceToGPU();
};
