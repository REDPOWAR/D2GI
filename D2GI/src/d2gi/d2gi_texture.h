#pragma once

#include "d2gi_surface.h"
#include "d2gi_mipmap_surf.h"


class D2GIPalette;


class D2GITexture : public D2GISurface
{
protected:
	DWORD m_dwMipMapCount;
	D2GIMipMapSurface** m_lpMipMapLevels;

	D3D9::IDirect3DTexture9* m_pTexture;

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

	D3D9::IDirect3DSurface9* GetD3D9Surface();
	D3D9::IDirect3DTexture9* GetD3D9Texture() { return m_pTexture; }
	DWORD GetMipMapCount() { return m_dwMipMapCount; }
};
