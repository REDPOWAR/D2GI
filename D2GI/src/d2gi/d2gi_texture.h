#pragma once

#include "d2gi_surface.h"
#include "d2gi_mipmap_surf.h"

#include <wrl/client.h>
#include <memory>

class D2GIPalette;
class D2GISystemMemorySurface;


class D2GITexture : public D2GISurface
{
protected:
	uint32_t m_dwMipMapCount = 1;
	std::unique_ptr<Microsoft::WRL::ComPtr<D2GIMipMapSurface>[]> m_lpMipMapLevels;

	Microsoft::WRL::ComPtr<D3D9::IDirect3DTexture9> m_pTexture;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> m_pLastBlitSource;

public:
	D2GITexture(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT, DWORD dwMipMapCount);
	virtual ~D2GITexture();

	virtual SURFACETYPE GetType() const override { return ST_TEXTURE; }
	virtual void LoadResource(bool bResettingDevice) override;
	virtual void ReleaseResource(bool bResettingDevice) override;

	STDMETHOD(SetColorKey)(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK) override;
	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(Unlock)(LPRECT);
	STDMETHOD(Blt)(LPRECT, D3D7::LPDIRECTDRAWSURFACE7, LPRECT, DWORD, D3D7::LPDDBLTFX);
	STDMETHOD(GetAttachedSurface)(D3D7::LPDDSCAPS2, D3D7::LPDIRECTDRAWSURFACE7 FAR*);
	STDMETHOD(GetSurfaceDesc)(D3D7::LPDDSURFACEDESC2);

	IFACEMETHOD(SetPrivateData)(REFGUID, LPVOID, DWORD, DWORD) override;
	IFACEMETHOD(GetPrivateData)(REFGUID, LPVOID, LPDWORD) override;
	IFACEMETHOD(FreePrivateData)(REFGUID) override;

	void SetBlitSource(const D2GISystemMemorySurface* pBlitSource);

	D3D9::IDirect3DSurface9* GetD3D9Surface();
	D3D9::IDirect3DTexture9* GetD3D9Texture();
	uint32_t GetMipMapCount() const { return m_dwMipMapCount; }

private:
	void EnsureD3DResourceCreated();
	void FlushResourceToGPU();
	bool IsDynamicTexture() const { return HasColorKeyConversion() || m_pLastBlitSource != nullptr; }
};
